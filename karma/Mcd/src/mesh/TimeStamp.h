/* -*-c++-*-
 *===============================================================
 * File:        TiemStamp.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.4.32.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef TimeStamp_H
#define TimeStamp_H

class     TimeStamp {
  public:
    TimeStamp() {
    mtime = 0;
    } void    advance();

    unsigned long int getTime() const {
    return mtime;
    }
    // not used=======================================================
    int operator > (TimeStamp & ts) {
    return (this->mtime > ts.mtime);
    };

    // not used
    int       operator < (TimeStamp & ts) {
    return (this->mtime < ts.mtime);
    };

  private:
    unsigned long mtime;    // modified time

};

inline void
          TimeStamp::advance()
{
    // 4 bytes, max: 4,294,967,295
    static unsigned long
    theGlobalTime_Count = 0;
    mtime = ++theGlobalTime_Count;

}

#endif              // TimeStamp
