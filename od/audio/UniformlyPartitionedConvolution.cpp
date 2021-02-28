/*
 * UniformlyPartitionedConvolution.cpp
 *
 *  Created on: 13 Sep 2017
 *      Author: clarkson
 */

#include <od/audio/UniformlyPartitionedConvolution.h>
#include <od/config.h>

namespace od
{

	UniformlyPartitionedConvolution::UniformlyPartitionedConvolution()
	{
		fftsize = FRAMELENGTH * 2;
		nbins = fftsize / 2 + 1;
		fft.allocate(fftsize);
		ir_prepared = false;

		// allocate buffer for DFT of padded input
		input_td = new float[fftsize];				// float array into which input block is copied and padded
		input_fd = new complex_float_t[nbins]; // fftwf_complex pointer to the same array to facilitate in-place fft

		// allocate a buffer into which to multiply the input by a partition and inverse-fft
		multbuf_td = new float[fftsize];
		multbuf_fd = new complex_float_t[nbins];
	}

	UniformlyPartitionedConvolution::~UniformlyPartitionedConvolution()
	{
		clearIR();

		delete[] input_td;
		delete[] input_fd;
		delete[] multbuf_td;
		delete[] multbuf_fd;
	}

	void UniformlyPartitionedConvolution::clearIR()
	{
		if (ir_prepared)
		{
			ir_prepared = false;

			for (int i = 0; i < nparts; i++)
			{
				delete[] irpart_fd[i];
			}

			delete[] outbuf;
		}
	}

	void UniformlyPartitionedConvolution::setIR(float *data, int n, int stride)
	{
		int i;
		int j;
		int k;

		clearIR();

		nparts = n / FRAMELENGTH;
		if (n % FRAMELENGTH != 0)
		{
			nparts++;
		}
		if (nparts > MAXPARTS)
		{
			nparts = MAXPARTS;
		}

		// allocate, fill, pad, and transform each IR partition
		float *td = new float[fftsize];
		for (k = 0, i = 0; i < nparts; i++)
		{
			irpart_fd[i] = new complex_float_t[nbins]; // fftwf_complex pointer to same array to facilitate in-place fft
			for (j = 0; j < FRAMELENGTH && k < n; j++, k++)
			{
				td[j] = data[k * stride];
			}
			for (; j < fftsize; j++)
			{
				td[j] = 0;
			}

			fft.compute(irpart_fd[i], td);
			// now, irpart_fd[i] contains the DFT of the ith partition of the impulse response.
		}
		delete[] td;

		// set up output ring buffer
		outbufsize = FRAMELENGTH * (nparts + 1);
		outbuf = new float[outbufsize];
		outbufpos = 0;
		for (i = 0; i < outbufsize; i++)
			outbuf[i] = 0;

		ir_prepared = true;
	}

	void UniformlyPartitionedConvolution::process(float *in, float *out)
	{
		int i;
		int j;
		int k; // bin
		int p; // partition

		if (!ir_prepared)
		{
			return;
		}

		// copy, pad, and transform the input block into input_td
		for (i = 0; i < FRAMELENGTH; i++)
		{
			input_td[i] = in[i];
		}
		for (; i < fftsize; i++)
		{
			input_td[i] = 0;
		}
		fft.compute(input_fd, input_td);

		// for each partition,
		for (p = 0; p < nparts; p++)
		{
			// multiply the input block by the partition, storing the result in multbuf
			for (k = 0; k < nbins; k++)
			{
				multbuf_fd[k].r = (input_fd[k].r * irpart_fd[p][k].r - input_fd[k].i * irpart_fd[p][k].i);
				multbuf_fd[k].i = (input_fd[k].r * irpart_fd[p][k].i + input_fd[k].i * irpart_fd[p][k].r);
			}
			// now, multbuf_fd contains the complex product of the DFT of the padded input block and the DFT of the pth padded IR partition.
			// transform the product:
			fft.computeInverse(multbuf_td, multbuf_fd); // FIXME accumulate in fd so only one fft and one ifft are needed per block
			// add multbuf_td into outbuf at the right place
			j = (outbufpos + (p * FRAMELENGTH)) % outbufsize;
			for (i = 0; i < fftsize; i++, j = (j + 1) % outbufsize)
			{
				outbuf[j] += multbuf_td[i];
			}
		}

		// output the current block of outbuf
		for (i = 0, j = outbufpos; i < FRAMELENGTH; i++, j = (j + 1) % outbufsize)
		{
			out[i] = outbuf[j];
			outbuf[j] = 0; // zero it after output
		}

		// increment outbufpos
		outbufpos += FRAMELENGTH;
		outbufpos %= outbufsize;
	}

} /* namespace od */
