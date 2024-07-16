# Weighted Population Compression

The data folder contains both our new data and our old data.  Invoking `make`
will reconstruct the new data if it does not exist already.  Other than
the result previously reported, intermediary automata over rationals always
have the same size as the corresponding "unweighted" automata.  This is
in accordance with the fact that the minimal Q-weighted automaton for a
Z-valued language has the same size as the minimal Z-weighted automaton for
that same language.  Buna-Marginean, et al. (2024) provide a recent
demonstration of this fact and according to them it was first proved in
Fliess (1974), which I cannot verify for lack of access (and presumably
a language barrier).

Alex Buna-Marginean, Vincent Cheval, Mahsa Shirmohammadi, and James Worrell. 2024. On Learning Polynomial Recursive Programs. Proc. ACM Program. Lang. 8, POPL, Article 34 (January 2024). https://doi.org/10.1145/3632876

Fliess. Matrices de Hankel (1974). J. Math. Pures Appl 53, 9 (1974), 197–222.
