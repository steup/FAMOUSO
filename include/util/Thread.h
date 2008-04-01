#ifndef __Thread_h__
#define __Thread_h__

class Thread
{
 public:
  Thread ();
  virtual ~Thread ();

  void start ();
  virtual void action () = 0;

  static unsigned int sleep(unsigned int);
 private:
  Thread (const Thread& ) {}
  Thread& operator=(const Thread& ) { return *this; }
};

#endif

