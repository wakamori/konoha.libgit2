#include <git2.h>

#define TRACE_ERROR(ctx, func, error) \
			KNH_NTRACE2(ctx, func, K_FAILED, \
				KNH_LDATA(LOG_i("errno", error), \
					LOG_s("git_lasterror", git_lasterror())))
