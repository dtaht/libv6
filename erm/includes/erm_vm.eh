// Use anonymous unions to represent the vm

typedef struct {
  union {
    u32 baseptr0;
    u32 reg1;
  };
  union {
    u32 baseptr1;
    u32 reg2;
  };
  union {
    u32 baseptr2;
    u32 reg3;
  };
  union {
    u32 baseptr3;
    u32 reg4;
  };
  union {
    u32 baseptr4;
    u32 reg5;
  };
  union {
    u32 ring1ptr;
    u32 reg6;
  };
  union {
    u32 ring2ptr;
    u32 reg7;
  };
  union {
    u32 msgbaseptr;
    u32 reg8;
  };
  union {
    u32 timer1;
    u32 reg9;
  };
  union {
    u32 timer2;
    u32 rega;
  };
}
