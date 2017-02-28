// NEON optimized memcpy for constant 8, 12 and 16 
// The default library routines punt here above 8
// And I wanted to use up neon regs

typedef uintbla t128 

#define memcpy(to,from,qt) qty == 16 ? memcpy16(to,from)\
			 : qty == 12 ? memcpy12(to,from) \
			 : qty == 8  ? memcpy8(to,from) \
			 : memcpy(to,from,qt);

// Neon specific short mcpys 

void *memcpy16(to,from) { u64x2 tmp = vld(from); vst(to,tmp); return to; }
void *memcpy12(to,from) { u64x2 tmp = vld(from); vst(to,tmp); return to; }
void *memcpy8(to,from) { u64x1 tmp = vld(from); vst(to,tmp); return to; }

# What I want to do is always for a constant literal, use neon and also
# prefetch

void *memset16zero(to) { u64x2 tmp = xor(tmp,tmp) write(tmp); 

memmov16() // can't pee on each other

memmov8
