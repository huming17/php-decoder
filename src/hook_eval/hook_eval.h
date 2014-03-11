#ifdef HOOK_EVAL_EXPORTS
#define HOOK_EVAL_API __declspec(dllexport)
#else
#define HOOK_EVAL_API __declspec(dllimport)
#endif

HOOK_EVAL_API void hook_eval(char *, unsigned long length);
