#ifndef _MAIN_H_
#define _MAIN_H_
// ref https://github.com/troydhanson/network/blob/master/unixdomain/05.dgram/recv.c
extern int main_dummy(int argc, char** argv);
#ifdef UTILS_UDS
	extern int main_uds(int argc, char** argv);
	#define MAIN_UDS	{ "main_uds", main_uds }
#else
	#define MAIN_UDS	{ "main_uds", main_dummy }
#endif
#ifdef UTILS_DB
	extern int main_db(int argc, char** argv);
	#define MAIN_DB		{ "main_db", main_db }
#else
	#define MAIN_DB		{ "main_db", main_dummy }
#endif
#ifdef UTILS_INPUT
	extern int main_input(int argc, char** argv);
	#define MAIN_INPUT	{ "main_input", main_input }
#else
	#define MAIN_INPUT	{ "main_input", main_dummy }
#endif
#ifdef UTILS_LXC
	extern int main_lxc_create(int argc, char** argv);
	#define MAIN_LXC_CREATE	{ "main_lxc_create", main_lxc_create }
#else
	#define MAIN_LXC_CREATE	{ "main_lxc_create", main_dummy }
#endif
#ifdef UTILS_DRM
	extern int main_drm(int argc, char** argv);
	#define MAIN_DRM { "main_drm", main_drm }
#else
	#define MAIN_DRM { "main_drm", main_dummy }
#endif

#ifdef UTILS_RFB
	extern int main_rfb(int argc, char** argv);
	#define MAIN_RFB { "main_rfb", main_rfb }
#else
	#define MAIN_RFB { "main_rfb", main_dummy }
#endif

#ifdef UTILS_QEMU
	extern int main_qmp(int argc, char** argv);
	#define MAIN_QMP { "main_qmp", main_qmp }
	extern int main_qemumonitor(int argc, char** argv);
	#define MAIN_QEMUMONITOR {"main_qemumonitor", main_qemumonitor }
#else
	#define MAIN_QMP { "main_qmp", main_dummy }
	#define MAIN_QEMUMONITOR {"main_qemumonitor", main_dummy }
#endif

extern int main_unittest(int argc, char** argv);
#define MAIN_UNITTEST	{ "main_unittest", main_unittest }

extern int main_md(int argc, char** argv);
#define MAIN_MD		{ "main_md", main_md }

#define MAIN_END	{ "", NULL }

#endif
