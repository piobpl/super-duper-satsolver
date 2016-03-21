/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <vector>

#include "../src/input.h"
#include "../src/output.h"
#include "../src/model.h"

int verify(const Model &model, const std::vector<Clause> clauses) {
  auto unsatisfiedCount =
    std::count_if(
      clauses.begin(),
      clauses.end(),
      [&model](const Clause &c){ return !model.satisfied(c); });
  if (unsatisfiedCount > 0) {
    std::cerr << unsatisfiedCount << " unsatisfied clauses\n";
    return -1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  FILE* testfile = nullptr;
  FILE* ansfile = nullptr;

  int n;
  std::vector<Clause> clauses;
  Model model;

  if (argc == 2) {
    testfile = fopen(argv[1], "r");
    ansfile = stdin;
  } else if (argc == 3) {
    testfile = fopen(argv[1], "r");
    ansfile = fopen(argv[2], "r");
  } else {
    std::cerr << "!Usage: verifier TEST [ANS]\n";
    exit(-1);
  }

  Input::read(testfile, &n, &clauses);
  Output::read(ansfile, n, &model);

  int exitCode = verify(model, clauses);

  fclose(testfile);
  fclose(ansfile);

  return exitCode;
}
