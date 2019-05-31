# Common defines applicable for all location-prop BB files

# enabling logcat for target which supports logd in both internal and external builds.
CFLAGS_append_mdm9607 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_mdm9607 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_mdm9607 = " -llog"

CFLAGS_append_apq8096 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_apq8096 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_apq8096 = " -llog"

CFLAGS_append_apq8098 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_apq8098 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_apq8098 = " -llog"

CFLAGS_append_qcs605 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_qcs605 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_qcs605 = " -llog"

CFLAGS_append_apq8017 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_apq8017 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_apq8017 = " -llog"

CFLAGS_append_msm8909 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_msm8909 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_msm8909 = " -llog"

CFLAGS_append_apq8053 = " -DUSE_ANDROID_LOGGING"
CPPFLAGS_append_apq8053 = " -DUSE_ANDROID_LOGGING"
LDFLAGS_append_apq8053 = " -llog"
