#ifndef CONFIG_H
#define CONFIG_H

#include <Qt/XConfig.h>
#include "filesync.h"

#define APP_CONFIG_LIST(_) \
    _(SrcDir		,"") \
    _(DstDir		,"") \
    _(CompareMode   ,FileSync::TimeAndSizeCompareMode) \
    _(SyncMode      ,FileSync::MirrorSyncMode)
X_DEF_CONFIG(FileSync,FileSync, APP_CONFIG_LIST)

#endif // CONFIG_H
