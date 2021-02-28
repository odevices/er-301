#include <hal/timing.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/a8/TimestampProvider.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#include <hal/log.h>

#define USE_RTC 0

typedef struct
{
  // clock measurement
  Types_FreqHz F;
  float F_period;
  unsigned int CPU_FREQ;
  float CPU_period;
  double CPU_periodD;
} Local;

static Local self;

tick_t ticks(void)
{
  Types_Timestamp64 time;
  Timestamp_get64(&time);
  return ((uint64_t)time.hi << 32) | (uint64_t)time.lo;
}

double ticks2secsD(tick_t nticks)
{
  return nticks * self.CPU_periodD;
}

float ticks2secs(tick_t nticks)
{
  return nticks * self.CPU_period;
}

uint64_t wallclock_offset = 0;
float wallclock(void)
{
  Types_Timestamp64 time;
  uint64_t t64;

  TimestampProvider_get64(&time);
  t64 = ((uint64_t)time.hi << 32) | time.lo;
  t64 -= wallclock_offset;

  return t64 * self.F_period;
}

void wallclock_reset(void)
{
  Types_Timestamp64 time;
  TimestampProvider_get64(&time);
  wallclock_offset = ((uint64_t)time.hi << 32) | time.lo;
}

unsigned long micros(void)
{
  Types_Timestamp64 time;
  uint64_t t64;

  Timestamp_get64(&time);
  t64 = ((uint64_t)time.hi << 32) | time.lo;
  return (t64 / (self.CPU_FREQ / 1000000));
}

#if USE_RTC
void Timing_init(void)
{
  unsigned int start_cpu, start_rtc, end_cpu, end_rtc, cnt = 0;
  Types_FreqHz f;
  logInfo("-----Timing Init (with RTC)--------------");

  while (!TimestampProvider_Module_startupDone())
  {
    cnt++;
  }
  wallclock_reset();
  logInfo("Checked for TimestampProvider startup done: %d times", cnt);

  UInt key = Hwi_disable();
  Clock_tickStop();
  Clock_tickReconfig();
  Clock_tickStart();
  Hwi_restore(key);

  TimestampProvider_getFreq(&self.F);
  self.F_period = 1.0f / self.F.lo;
  logInfo("TimestampProvider Freq: %d Hz", (int)self.F.lo);
  logInfo("Clock tick period: %d us", Clock_tickPeriod);

  // check the cpu freq against the RTC
  start_cpu = Timestamp_get32();
  start_rtc = TimestampProvider_get32();
  while (TimestampProvider_get32() - start_rtc < self.F.lo)
  {
    Task_sleep(10);
  }
  end_rtc = TimestampProvider_get32();
  end_cpu = Timestamp_get32();

  // float version
  float cpu_freq, dt;
  dt = (end_rtc - start_rtc);
  dt *= 1.0 / self.F.lo;
  cpu_freq = (end_cpu - start_cpu) / dt;
  logInfo("Measured CPU Freq: %d MHz", (int)(cpu_freq / 1e6));
  self.CPU_FREQ = (int)cpu_freq;
  self.CPU_period = 1.0f / cpu_freq;

  // double version
  double cpu_freqD, dtD;
  dtD = (end_rtc - start_rtc);
  dtD *= 1.0 / self.F.lo;
  cpu_freqD = (end_cpu - start_cpu) / dtD;
  self.CPU_periodD = 1.0 / cpu_freqD;

  f.lo = self.CPU_FREQ;
  BIOS_setCpuFreq(&f);
  logInfo("BIOS_setCpuFreq(%d Hz)", (int)f.lo);
  BIOS_getCpuFreq(&f);
  logInfo("BIOS_getCpuFreq() returns: %d Hz", (int)f.lo);

  logInfo("---------------------------------");
}

#else
void Timing_init(void)
{
  int cnt = 0;
  Types_FreqHz f;
  logInfo("--------Timing Init--------------");

  while (!TimestampProvider_Module_startupDone())
  {
    cnt++;
  }
  wallclock_reset();
  logInfo("Checked for TimestampProvider startup done: %d times", cnt);

  UInt key = Hwi_disable();
  Clock_tickStop();
  Clock_tickReconfig();
  Clock_tickStart();
  Hwi_restore(key);

  TimestampProvider_getFreq(&self.F);
  self.F_period = 1.0f / self.F.lo;
  logInfo("TimestampProvider Freq: %d Hz", (int)self.F.lo);
  logInfo("Clock tick period: %d us", Clock_tickPeriod);

  // float version
  float cpu_freq;
  cpu_freq = 1000000000;
  self.CPU_FREQ = (int)cpu_freq;
  self.CPU_period = 1.0f / cpu_freq;

  // double version
  double cpu_freqD;
  cpu_freqD = 1000000000;
  self.CPU_periodD = 1.0 / cpu_freqD;

  f.lo = self.CPU_FREQ;
  BIOS_setCpuFreq(&f);
  logInfo("BIOS_setCpuFreq(%d Hz)", (int)f.lo);
  BIOS_getCpuFreq(&f);
  logInfo("BIOS_getCpuFreq() returns: %d Hz", (int)f.lo);

  logInfo("---------------------------------");
}

#endif

void test_sleep()
{
  logInfo("-----------Test Task_sleep-------");
  for (int i = 0; i < 10; i++)
  {
    logInfo("%d ", i);
    Task_sleep(1000);
  }
  logInfo("---------------------------------");
}