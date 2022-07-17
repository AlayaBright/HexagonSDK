int third() { return 0; }
int second() { return third(); }
int first() { return second() + third(); }

