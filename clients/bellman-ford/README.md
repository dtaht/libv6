# Solving Bellman-Ford *Fast*

There is a ton of lit on optimizing the bellman-ford algorithm. It
seems to be a classic thing for PHDs to attempt.

Of note (to me) are the incremental,
parallized versions that are suitable for multicores and GPUS, and
there's really good stuff about doing it directly in FPGAs, also.

What's lacking in nearly all of them is actual, reusable source code.

Both major edge finding algorithms (Djikstra) and BF, have severe scaling
problems. Not being able to do it fast is one of the major blockers for 
BGP.

Worse, nearly everybody in deployed code uses a mildly modified version
to handle some needed special characteristic of the routing protocol,
which can blow up some basic assumptions in all the lit.

So...

What data structures are amiable to doing bellman-ford faster... today?

How can I speed up (using co-processors, an FPGA assist), a BF algorithm
like babel's or BGP's to scale to millions of nodes?

One thing that's not well understood about these algos is that they are
potentially embarrassingly parallel.

## Aside: On Snarks

I have long held an opinion about how we are solving for edges is basically...
wrong. We see all sorts of optimizations nowadays solving for the 1% of
routes that matter, tossing that into hardware, and then offloading the rest
into software.

We don't need to solve for all paths, just 3 "good" paths (simultaneously) that meet certain goals - this is a good path for voice, this for bulk, this one is in case the network interface goes down. Or solve just the paths you are going to hit hit a lot, and then punt to userspace.

My own inspiration from graph theory are a set of solutions for
"good paths" that fall out of the [snark concept](https://en.wikipedia.org/wiki/Blanu%C5%A1a_snarks). I felt so strongly about that that I made a snark the
semi-official logo of the CeroWrt project, and I figured - that once we'd
spent a few weeks fixing bufferbloat (hah!), and IPv6, I'd be able to get back
on what I'd cared about.

The characteristic polynomial of a snark (which is descriptive rather than
prescriptive so not of huge use except as "an ideal goal") is something that
can also be solved MUCH faster with modern processors than ever before
but nobody's bothered.

It's also very amiable to a whole bunch of DSPs laboring away in
parallel. (but nobody's bothered).

## Anyway...

What I'd like to do here is a bunch of toys for solving BF. In particular,
I'd like to fiddle with babel's specific implementation (which solves 3
thorny problems in the first place), and see what structures fit it best.

I've long been on a quest for a stable RTT based routing metric *that worked*.
Much progress on this old line of research has been made lately, and with the
invention of the fq_codel algorithm, all sorts of latencies and jitter drop
out of the noise, and some of the characteristics of fq_codel can be
leveraged to extract information from the noise.

