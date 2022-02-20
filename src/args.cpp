#include "base.h"
#include <gflags/gflags.h>

DEFINE_string(cmd, "", "command");
DEFINE_string(cwd, "", "command work directory");

DEFINE_bool(debug, false, "debug log");
DEFINE_string(logFile, "", "logFile path");
DEFINE_bool(logIsAppend, true, "logFile is append content");
DEFINE_bool(logIsLogHead, true, "logFile is log head");
DEFINE_int64(logLimit, 10 * 1024 * 1024, "logFile limit size");

CmdArgs parseCmdArgs(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    CmdArgs args;
    args.cmd = FLAGS_cmd;
    args.cwd = FLAGS_cwd;

    args.debug = FLAGS_debug;
    args.logFile = FLAGS_logFile;
    args.logIsAppend = FLAGS_logIsAppend;
    args.logIsLogHead = FLAGS_logIsLogHead;
    args.logLimit = FLAGS_logLimit;

    return args;
}
