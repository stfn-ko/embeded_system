/*\ ---------------------------------------------
|*| @name: LOGTASK.H
|*| @INFO: FOR CONTEXT CHECK OUT INCLUDE FILES
\*/

#include "LogTask.h"

#define IIC_ADDR_RTC 0xDE

typedef struct _RTCDATEREGS
{

  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t wkday;
  uint8_t date;
  uint8_t month;
  uint8_t year;

} RTCDATEREGS;
typedef RTCDATEREGS *PRTCDATEREGS;
/*\ ---------------------------------------------
|*| @name: LogTask
|*| @description: Constructor. This is called automatically when an instance of the
|*| class is created. It initializes the timer.
|*| @note: we don't pass a timeout to the constructor. The timeout
|*| is hardcoded in the class. Ask yourself: is this a good idea?
|*| Maybe we will need to change the timeout dynamically?
|*| Actually we may use a different mechanism to trigger a log.
\*/
LogTask::LogTask(int _timestamp) : tm(_timestamp)
{
  // Now initialize the RTC device and start it running
  unsigned char iicregs[2] = {0x00, 0x00};
  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_RTC, iicregs, 2); // write the address.
}

/*\ ---------------------------------------------
|*| @name: SetDate
|*| @description: This function can be called to set the date and time on the I2C
|*| RTC internal clock.
|*| @scope: PUBLIC
|*| @context: TASK
|*| @param: NONE
|*| @return: 0 success, nonzero error
\*/

int LogTask::SetDate(int dow, int day, int month, int year, int hrs, int mins, int secs, boolean is24hr, boolean ampm)
{
  struct _iicdrwrite
  {
    uint8_t address;
    RTCDATEREGS regs;
  } date;

  int rc = 1;
  // we must first error-check our input.
  const int maxmonthdays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  do
  {

    if (dow < 1 || dow > 7)
      break;

    if (month < 1 || month > 12)
      break;

    if (day < 1 || day > maxmonthdays[month - 1])
      break;

    if (year < 0 || year > 99)
      break;

    if (!is24hr && (hrs < 1 || hrs > 12))
      break;
    else if (hrs < 0 || hrs > 23)
      break;

    if (mins < 0 || mins > 59)
      break;

    if (secs < 0 || secs > 59)
      break;

    date.address = 0;
    date.regs.sec = ((secs % 10) | ((secs / 10) << 4)) | 0x80; // check
    date.regs.min = ((mins % 10) | ((mins / 10) << 4)) | 0x80; // check

    if (is24hr)
      date.regs.hour = ((hrs % 10) | ((hrs / 10) << 4)); // 24 format check
    else if (ampm)
      date.regs.hour = ((hrs % 10) | ((hrs / 10) << 4)) | 0x40; // 12 format am check
    else
      date.regs.hour = ((hrs % 10) | ((hrs / 10) << 4)) | 0x60; // 12 format pm check

    date.regs.wkday = (dow) | 0x20;                         // check
    date.regs.date = ((day % 10) | ((day / 10) << 4));      // check
    date.regs.month = ((month % 10) | ((month / 10) << 4)); // add leap year thing
    date.regs.year = ((year % 10) | ((year / 10) << 4));    // check

    // Writing the date data to the RTC
    rc = Kernel::OS.IICDriver.IICWrite(IIC_ADDR_RTC, (unsigned char *)&date, sizeof(struct _iicdrwrite)); // write the address.

  } while (0);

  return rc;
}

/*\ ---------------------------------------------
|*| @name: TaskLoop
|*| @description: Main task loop - this is overloaded from class Task and performs
|*| the deed!
|*| @scope: PUBLIC
|*| @context: TASK
|*| @param: none
|*| @return: none
\*/
void LogTask::TaskLoop()
{
  if (tm.isExpired())
  {

    LogToSerial((char *)"log message");

    tm.Restart();
  }
}

/*\ ---------------------------------------------
|*| @name: LogToSerial
|*| @TASK: Create a timestamped log string and write it to the serial port
|*| @scope: PRIVATE
|*| @context: TASK
|*| @param: message string (<=127 characters) to write to the log
|*| @return: none
\*/
void LogTask::LogToSerial(char *message)
{
  RTCDATEREGS date;
  const char *days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  char logstring[200];
  int hours, minutes, seconds;
  char *amIndicator;
  char *weekday;
  int day, month, year;
  uint8_t iicregs[2];

  // read date from RTC
  iicregs[0] = 0x00; // RTCSEC register address
  iicregs[1] = 0;
  Kernel::OS.IICDriver.IICWrite(IIC_ADDR_RTC, iicregs, 1);                                 // write the address.
  Kernel::OS.IICDriver.IICRead(IIC_ADDR_RTC, (unsigned char *)&date, sizeof(RTCDATEREGS)); // read the values

  // now massage to convert from BCD to decimal and write to a string

  boolean is12hr = (date.hour & 0b01000000);
  boolean ampm = (date.hour & 0b00100000);

  seconds = (date.sec & 0x0f) + (10 * ((date.sec >> 4) & 0x07));   // check
  minutes = (date.min & 0x0f) + (10 * ((date.min >> 4) & 0x07));   // check
  day = (date.date & 0x0f) + (10 * ((date.date >> 4) & 0x07));     // check
  weekday = const_cast<char *>(days[(date.wkday & 0x0f) - 1]);     // check // edited to remove warning 'invalid conversion from 'const char*' to 'char*'
  month = (date.month & 0x0f) + (10 * ((date.month >> 4) & 0x07)); // check
  year = (date.year & 0x0f) + (10 * (date.year >> 4));             // check

  hours = (date.hour & 0x0f) + (10 * ((date.hour >> 4) & 0x07));
  amIndicator = const_cast<char *>("");
  if (is12hr)
  {
    hours = (date.hour & 0x0f) + (10 * ((date.hour >> 4) & 0x01));
    amIndicator = const_cast<char *>("pm"); // edit to_check
    if (ampm)
      amIndicator = const_cast<char *>("am"); // edit to_check
  }

  // Using sprintf to create a string to send to the serial port.
  // Note how the message passed as a parameter to this function is appended
  sprintf(logstring, "%s %d:%02d:%02d %s%d/%02d/%02d: %.127s\n", weekday, hours, minutes, seconds, amIndicator, day, month, year, message);

  // Isn't this approach better than having multiple Serial.write() calls!
  Serial.write(logstring);
}
