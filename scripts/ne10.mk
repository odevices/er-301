include scripts/env.mk

LIBNAME := ne10
src_dir = $(ne10_dir)
includes += $(ne10_dir)/inc $(ne10_dir)/common $(arch_dir)
symbols += NE10_ENABLE_MATH
symbols += NE10_ENABLE_DSP
symbols += NE10_ENABLE_HF
symbols += ENABLE_NE10_FIR_FLOAT_NEON
symbols += ENABLE_NE10_FIR_DECIMATE_FLOAT_NEON
symbols += ENABLE_NE10_FIR_INTERPOLATE_FLOAT_NEON
symbols += ENABLE_NE10_FIR_SPARSE_FLOAT_NEON
symbols += ENABLE_NE10_FIR_LATTICE_FLOAT_NEON
symbols += ENABLE_NE10_IIR_LATTICE_FLOAT_NEON

excludes = NE10_fft_float32.neonintrinsic.c \
		  NE10_fft_int16.neonintrinsic.c \
		  NE10_fft_int32.neonintrinsic.c \
		  NE10_rfft_float32.neonintrinsic.c \
			NE10_fft_float32.neonv8.S

CFLAGS += -Wno-unused-variable -Wno-maybe-uninitialized 

include scripts/lib-builder.mk
