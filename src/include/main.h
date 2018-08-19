#ifndef _MAIN_H_
#define _MAIN_H_
// ref https://github.com/troydhanson/network/blob/master/unixdomain/05.dgram/recv.c
#ifdef UTILS_UDS
	extern int main_uds(int argc, char** argv);
	#define MAIN_UDS	{ "main_uds", main_uds },
#else
	#define MAIN_UDS
#endif
#ifdef UTILS_DB
	extern int main_db(int argc, char** argv);
	#define MAIN_DB		{ "main_db", main_db },
#else
	#define MAIN_DB
#endif
#ifdef UTILS_INPUT
	extern int main_input(int argc, char** argv);
	#define MAIN_INPUT	{ "main_input", main_input },
#else
	#define MAIN_INPUT
#endif
#ifdef UTILS_LXC
	extern int main_lxc_create(int argc, char** argv);
	#define MAIN_LXC_CREATE	{ "main_lxc_create", main_lxc_create },
#else
	#define MAIN_LXC_CREATE
#endif
#ifdef UTILS_DRM
	extern int main_drm(int argc, char** argv);
	#define MAIN_DRM { "main_drm", main_drm }, 
#else
	#define MAIN_DRM
#endif

#ifdef UTILS_RFB
	extern int main_rfb(int argc, char** argv);
	#define MAIN_RFB { "main_rfb", main_rfb },
#else
	#define MAIN_RFB
#endif

#ifdef UTILS_QMP
	extern int main_qmp(int argc, char** argv);
	#define MAIN_QMP { "main_qmp", main_qmp },
#else
	#define MAIN_QMP
#endif

#define MAIN_END	{ "", NULL }

#endif
