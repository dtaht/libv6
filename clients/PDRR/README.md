# Passive RTT measurements and Parallel FQ-Codel DRR

We are pretty close to where all the observed overheads of core bits
of our FQ-AQM technologies can vanish into hardware, and instead of doing them
out of band, do them continually, and in hardware. What does this get us
in terms of better routing?

Timestamping: Can be done at the head of the packet.
Hashing: We could do a hash of all relevant fields and have that at the end of
the packet. You don't care about queuing if your destination is already clear,
so you just waste the hash when not queuing.

The hardware actually could do hashing in *every useful way* and have that at the end of the packet.

# Question:

Once you've got a useful hash and timestamp *everywhere* of every flow,
what else can you get?

It's not particularly well understood, I think, that once you have 1024 queues
you can solve for core, cache-missy things (mac table lookup, route table lookup), in parallel, and more lazily, and just vote as to who to deliver next.

(DRR is not inherently serial - just the hashing step is)

Similarly, with hashing headers, you might be able to get away from your classic
trie structure...

And the original fq-pie (the one based on sch_fq) had some *wonderful*
ideas in it....
