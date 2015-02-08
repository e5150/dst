enum SPKG { SPKG_NULL, SPKG_NAME, SPKG_VERSION, SPKG_ARCH, SPKG_BUILD, SPKG_TAG, SPKG_EXT, SPKG_BASE };
char *dst_packname(const char*, enum SPKG);
