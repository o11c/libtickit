void plan_tests(int n);
void ok(int cmp, char *name);
void pass(char *name);
void fail(char *name);
void diag(char *fmt, ...);
void is_int(int got, int expect, char *name);
void is_str(const char *got, const char *expect, char *name);
void is_str_escape(const char *got, const char *expect, char *name);
int exit_status(void);
