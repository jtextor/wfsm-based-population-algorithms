This repository will contain the code for the following paper by Gijs Schröder, Inge Wortel, and Johannes Textor:

# Population-Based Algorithms Built on Weighted Automata

  Many algorithms in natural computing and computational biology are
population-based: genetic algorithms evolve candidate solutions for
optimization problems; artificial immune systems and learning classifier
systems maintain populations of rules. Using such algorithms at very large
population sizes (e.g., millions or billions) is computationally
expensive.  Here, we develop a methodology for implementing population-based
models using  weighted finite state machines (WFSMs) with exact rational
weights.  For populations that can be represented as weighted sets of strings,
WFSMs can reduce memory use and runtime of population-based algorithms by
orders of magnitude.  We demonstrate the generality of our approach by
constructing an immune-inspired anomaly detector for string data and an
evolutionary algorithm that solves Boolean satisfiability problems.  The WFSM
approach allows repurposing of advanced algorithms developed for natural
language processing, and should be applicable to other population-based
algorithms such as learning classifier systems.

