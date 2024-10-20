/*
   The Swiss File Knife Command Line Multi Function Tool
   =====================================================
   StahlWorks Technologies, http://stahlworks.com/
   Provided under the BSD license.

   The whole source code was created with Depeche View,
   the world's fastest source code browser and editor.

   Windows build:
      cl /Fesfk.exe sfk.cpp sfkext.cpp sfkpack.cpp kernel32.lib user32.lib gdi32.lib ws2_32.lib advapi32.lib shell32.lib
      g++ -osfk.exe sfk.cpp sfkext.cpp sfkpack.cpp -lkernel32 -luser32 -lgdi32 -lws2_32 -ladvapi32 -lshell32

   Linux build:
      g++ -s sfk.cpp sfkext.cpp sfkpack.cpp -o sfk -Wformat-overflow=0 -Wformat-truncation=0

   See changes.txt for the version history.
*/

#define  MTKTRACE_CODE
#include "sfkbase.hpp"

#if (defined(SFKWEB) || defined(SFKPIC) || defined(SFK_PROFILING))
 #include "sfkint.hpp"
#endif

#include "sfkext.hpp"


#ifdef _WIN32
 #define getcwd _getcwd
 #define rmdir  _rmdir
#endif



#define USE_SFT_UPDATE

#define SFK_BOTH_RUNCHARS

#ifdef _WIN32
 #define SFK_BOTH_RUNCHARS_HELP
 #define SFK_MAP_ANSI_NEW // sfk189
#endif

// #define SFK_STRICT_MATCH
#define SFKVAR // 1770

#ifdef SFKINT2
 #define SFKPRINTREDIR
 #define SFKEXTERR
#endif

// should you get problems with fsetpos/fgetpos compile,
// activate this to disable zip/jar file content listing:
// #define NO_ZIP_LIST

#define snprintf  mysnprintf
#define sprintf   mysprintf



#ifndef SFK_VERSION
   #define SFK_VERSION  "0"
   #define SFK_FIXPACK  "0"
   #define VER_STR_OS   "0"
   const char *getPureSFKVersion() { return SFK_VERSION; }
#endif


#ifdef _WIN32
 #ifdef SFK_MEMTRACE
  #include "memdeb.cpp"
 #endif
#endif
#ifndef SFK_MEMTRACE
void sfkmem_checklist(const char *pszCheckPoint) { }
#endif

#ifdef _WIN32

      char  glblNotChar     = '!';
      char  glblRunChar     = '$';
      char  glblWildChar    = '*';

const char  glblPathChar    = '\\';
const char  glblWrongPChar  = '/';
const char *glblPathStr     = "\\";
const char *glblDubPathStr  = "\\\\";
const char *glblDotSlash    = ".\\";

// Windows default compile has a stack size of 1 mb.

#else

      char  glblNotChar     = ':';
      char  glblRunChar     = '#';
      char  glblWildChar    = '%';

const char  glblPathChar    = '/';
const char  glblWrongPChar  = '\\';
const char *glblPathStr     = "/";
const char *glblDubPathStr  = "//";
const char *glblDotSlash    = "./";

// Most linux have a stack larger than 1 mb, some smaller.
#define SFKBIGSTACK  // linux default

#endif

// - - - sfk internal types

#ifdef _WIN32
   #ifdef _MSC_VER
      #ifdef SFK_W64
      typedef struct __stat64 sfkstat_t;
      #else
      typedef struct stat sfkstat_t;
      #endif
   #else
      typedef struct stat sfkstat_t;
   #endif
#else
   typedef struct stat64 sfkstat_t;
#endif

// - - - sfk global vars

#if defined(SFKOFFICE)
bool bGlblOffice = 1;
#else
bool bGlblOffice = 0;
#endif
bool glblUPatMode = 0;
void *pGlblStartStack = 0;
int iGlblInScript = 0;
int nGlblShellRC = 0;
int nGlblFunc         = 0;
bool bGlblSyntaxTest  = 0;
int  bGlblCollectHelp = 0;
int  iGlblCollectCmd  = 0;
int nGlblActiveFileAgeLimit = 30; // days
int nGlblErrors   = 0; // perr counter
int nGlblWarnings = 0;
int nGlblTraceSel = 0; // b0:dirs b1:files
bool bGlblMD5RelNames = 0;
bool bGlblHaveInteractiveConsole = 0;
bool bGlblStartedInEmptyConsole = 0;
// bool bGlblEnableOPrintf = 1; // allow codepage conversion w/in oprintf
// bool bGlblForceCConv = 0;    // enfore codepage conversion w/in oprintf
bool bGlblAllowGeneralPure = 0; // command dependent
bool bGlblPauseOnError = 0;  // pause after every error
bool bGlblPauseOnEnd   = 0;  // pause before program end
bool bGlblOldMD5 = 0;
char *pszGlblOutFile = 0;  // if set, some funcs will take care not to read this file
char *pszGlblSaveTo  = 0;  // if set, some funcs will save output files to this path
FILE *fGlblOut     = 0; // general use

// fwrite on windows network drives may fail with blocks > 60 MB,
// therefore a single block write is limited to this size:
#define SFK_IO_BLOCK_SIZE 10000000 // about 10 MB

#define MY_GETBUF_MAX ((MAX_LINE_LEN+10)*5)

unsigned char abBuf[MAX_ABBUF_SIZE+100];
char szLineBuf[MAX_LINE_LEN+10];
char szLineBuf2[MAX_LINE_LEN+10];
char szLineBuf3[MAX_LINE_LEN+10];
char szAttrBuf[MAX_LINE_LEN+10];
char szAttrBuf2[MAX_LINE_LEN+10];
char szAttrBuf3[MAX_LINE_LEN+10];
char szRefNameBuf[MAX_LINE_LEN+10];
char szRefNameBuf2[MAX_LINE_LEN+10];
char szOutNameRecent[MAX_LINE_LEN+10];
char szPrintBuf1[MAX_LINE_LEN+10];
char szPrintBuf2[MAX_LINE_LEN+10];
char szPrintAttr[MAX_LINE_LEN+10];
char szPrintBufMap[MAX_LINE_LEN+10];
char szTopURLBuf[MAX_LINE_LEN+10];
char szIOTraceBuf[200];
bool bGlblToldAboutRecent = 0;
char szOutNameBuf[MAX_LINE_LEN+10];
char szRunCmdBuf[MAX_LINE_LEN+10];
#define MAX_MATCH_BUF 500
char szMatchBuf[MAX_MATCH_BUF+10];  // strmatch word buffer
bool szMatchEsc[MAX_MATCH_BUF+10];  // strmatch escape flags
char *pszGlblPreRoot = 0;
char szGlblMixRoot[MAX_LINE_LEN+10];
char *pGlblDumpBuf = 0;
int   iGlblDumpBufSize = 0;
bool bErrBufSet = 0;
cchar *pszGlblJamPrefix = ":file:";
char *pszGlblJamRoot = 0;
int   bGlblJamPure = 0;
#define MAX_JAM_TARGETS 1000
char *apJamTargets[MAX_JAM_TARGETS];
num   alJamTargetTime[MAX_JAM_TARGETS];
num   nJamSnapTime = -1;
int  nJamTargets  = 0;
char *pszGlblRepSrc = 0;
char *pszGlblRepDst = 0;
cchar *pszGlblBlank =
   "                                                "
   "                                                ";
num  nGlblStartTime = 0;
num  nGlblListMinSize = 0; // in bytes
int nGlblListMode = 0;    // 1==stat 2==list
int  nGlblListDigits = 12;
bool bGlblDisableEscape = 0;
bool bGlblEscape = 0;
bool bGlblEnter = 0;
char *pszGlblDstRoot = 0;
char *pszGlblDirTimes = 0;
cchar *pszGlblTurn = "\\|/-";
int  nGlblTurnCnt = 0;
bool  bGlblQuoted     = 0; // list: add quotes around filenames
bool  bGlblNoRootDirFiles = 0; // list -dir +dirmask
uint nGlblConvTarget = 0; // see eConvTargetFormats
uint aGlblConvStat[10];
bool  bGlblRefRelCmp    = 1;
bool  bGlblRefBaseCmp   = 0;
bool  bGlblRefWideInfo  = 0;
int  nGlblRefMaxSrc    = 10;
bool  bGlblRefLimitReached = 0;
bool  bGlblStdInAny     = 0;  // all cmd except run: take list from stdin
bool  bGlblStdInFiles   = 0;  // run only: take filename list from stdin
bool  bGlblStdInDirs    = 0;  // run only: take directory list from stdin
int  nGlblMD5Skip      = 0;
bool  bGlblMirrorByDate = 0;  // inofficial, might be removed.
int nGlblTCPMaxSizeMB   = 500; // MB
SOCKET hGlblTCPOutSocket = 0;
bool bGlblFTPReadWrite   = 0;
bool bGlblFTPListFlatTS  = 0; // server: send flat timestamp on list
bool bGlblFTPListAsHTML  = 0;
bool bGlblFTPListTextBin = 0;
bool bGlblBinGrep           = 0;
bool bGlblBinGrepAutoDetect = 1;
int nGlblDarkColBase    = 0;
int nGlblBrightColBase  = 1;
bool bGlblSysErrDetail  = 0;


// highlight=1 red=2 green=4 blue=8
#ifdef _WIN32
// windows default safety colors for ANY background.
// will be changed automatically if black background is detected.
// help part
int nGlblHeadColor      =  5; // green
int nGlblExampColor     = 11; // purple
// functional part
int nGlblFileColor      = 12; // cyan
int nGlblLinkColor      = 12; // cyan
int nGlblHitColor       =  5; // green
int nGlblRepColor       = 11; // purple
int nGlblErrColor       =  3; // red
int nGlblWarnColor      = 11; // purple
int nGlblPreColor       = 12; // cyan
int nGlblTimeColor      = 12; // cyan
int nGlblTraceIncColor  = 12; // cyan
int nGlblTraceExcColor  = 11; // purple
#else
// unix default colors for white background
int nGlblDefColor       =  0; // default
int nGlblHeadColor      =  4; // green
int nGlblExampColor     = 10; // purple
// functional part
int nGlblFileColor      = 10; // purple
int nGlblLinkColor      = 12; // cyan
int nGlblHitColor       =  4; // green
int nGlblRepColor       = 12; // blue
int nGlblErrColor       =  2; // red
int nGlblWarnColor      =  2; // red
int nGlblPreColor       = 12; // blue
int nGlblTimeColor      = 12; // blue
int nGlblTraceIncColor  = 12; // cyan
int nGlblTraceExcColor  = 11; // purple
#endif

bool bGlblGrepLineNum    = 0;
bool bGlblHtml           = 0;  // for html help creation
// bool bGlblShortSyntax    = 0;
// bool bGlblAnyUsed        = 0;
bool bGlblAllowAllPlusPosFile = 0;
char *pszGlblSinceDir    = 0;
bool bGlblSinceDirIncRef = 0;
int nGlblMissingRefDirs  = 0;
int nGlblMatchingRefDirs = 0;
int nGlblSinceMode       = 0; // b0:add b1:dif
bool bGlblIgnoreTime     = 0;
bool bGlblIgnore3600     = 0;
char  *pszGlblCopySrc    = 0;
char  *pszGlblCopyDst    = 0;
uchar *pGlblWorkBuf      = 0;
num    nGlblWorkBufSize  = 0;
int   nGlblCopyStyle     = 2; // how filenames are dumped onto terminal
int   nGlblCopyShadows   = 0;
num   nGlblShadowSizeLimit = 0;
bool  bGlblUseCopyCache  = 0;
bool  bGlblShowSyncDiff  = 0;
bool  bGlblHavePlusDirMasks = 0; // deprecated
num   nGlblMemLimit      = 300 * 1048576;
bool  bGlblMemLimitWasSet = 0;
bool  bGlblNoMemCheck     = 0;
bool  bGlblSFKCreateFiles = 0;
char  *pGlblCurrentScript = 0; // while within a script command
char  *pGlblCurScriptName = 0; // and it's file name

bool bGlblHexDumpWide    = 0;
int nGlblHexDumpForm     = 0;
num  nGlblHexDumpOff     = 0;
num  nGlblHexDumpLen     = 0;

#ifdef _WIN32
cchar *pszGlblAliasBatchHead = "@rem sfk alias batch";
#else
cchar *pszGlblAliasBatchHead = "# sfk alias batch";
#endif

int nGlblFzMisArcFiles = 0;
int nGlblFzConArcFiles = 0;
int nGlblFzConArchives = 0;
int nGlblFzMisCopFiles = 0;
int nGlblFzConCopFiles = 0;

int   nGlblConsColumns    = 80;
int   bGlblConsColumnsSet =  0;
int   nGlblConsRows       = 30;
int   bGlblConsRowsSet    =  0;

#ifdef _WIN32
HANDLE hGlblConsole     =  0;
WORD   nGlblConsAttrib  =  0;
HANDLE hGlblStdIn       =  0;
DWORD  nGlblStdRead     =  0;
#endif

struct CommandStats gs;    // global settings accross whole chain
struct CommandStats cs;    // command local statistics or settings
struct CommandStats cspre; // of previous command
struct CommandStats dummyCommandStats;

Array glblGrepPat("grep");
Array glblUnzipMask("unzip");
Array glblIncBin("incbin");

StringTable glblErrorLog;
StringTable glblStaleLog;

int perr(const char *pszFormat, ...);
int pwarn(const char *pszFormat, ...);
int pinf(const char *pszFormat, ...);

void logError(const char *format, ...)
{
   va_list arg_ptr;
   va_start(arg_ptr, format);

   char szTmpBuf[4096];
   vsprintf(szTmpBuf, format, arg_ptr);

   glblErrorLog.addEntry(szTmpBuf);
   printf("%s\n", szTmpBuf);
}

FileSet  glblFileSet;   // int format -dir and -file set
CoiTable glblSFL;       // short format specific file list
CoiTable glblFileListCache;

int glblSFLNumberOfEntries() { return glblSFL.numberOfEntries(); }

CommandChaining chain;

num getTotalBytes()    { return cs.totalbytes; }
void clearTotalBytes() { cs.totalbytes=0; }

bool infoAllowed() {
   if (cs.quiet)  return 0;
   if (cs.noinfo) return 0;
   return 1;
}

int quietMode() { return cs.quiet; }
int fastMode()  { return cs.fast;  }

bool vname() { return cs.uname || cs.tname || cs.aname; }

#ifdef VFILEBASE
void setxelike(bool byes) { gs.xelike = cs.xelike = byes; }
#endif // VFILEBASE

void setArcTravel(bool bYesNo, bool bPreCache, int iProbeFiles)
{
   gs.travelzips  = bYesNo;
   cs.travelzips  = bYesNo;
   if (iProbeFiles != 2)
   {
      gs.probefiles  = iProbeFiles;
      cs.probefiles  = iProbeFiles;
   }
   #ifdef VFILEBASE
   cs.precachezip = bPreCache;
   #endif // VFILEBASE
}

bool getArcTravel( ) { return cs.travelzips; }

// for dview
void setLoadOffice(int iYesNo) {
   gs.office = iYesNo;
   cs.office = iYesNo;
   if (!iYesNo) {
      gs.justoffice = 0;
      cs.justoffice = 0;
   }
}

// for dview
void setLoadJustOffice(int iYesNo) {
   gs.justoffice = iYesNo;
   cs.justoffice = iYesNo;
}

void setSubLoad(bool bYesNo) {
   mtklog(("setsubload %d", bYesNo));
   gs.subdirs = bYesNo;
   gs.maxsub  = 0;
   cs.subdirs = bYesNo;
   cs.maxsub  = 0;
}

void setHiddenLoad(bool bYesNo) {
   mtklog(("sethidload %d", bYesNo));
   gs.hidden = bYesNo;
   cs.hidden = bYesNo;
}

void setBinaryLoad(bool bYesNo) {
   mtklog(("setbinload %d", bYesNo));
   gs.incbin = bYesNo;
   cs.incbin = bYesNo;
}

void setUTFLoad(bool bYesNo) {
   mtklog(("setutfload %d", bYesNo));
   gs.wchardec = bYesNo;
   cs.wchardec = bYesNo;
}

int getWrapLoad(bool &rrewrap) {
   rrewrap = cs.rewrap;
   return cs.wrapcol;
}
void setWrapLoad(int n, bool brewrap) {
   cs.wrapcol = n;
   gs.wrapcol = n;
   cs.wrapbincol = (n >= 80) ? ((n * 90) / 100 - 10) : 80;
   gs.wrapbincol = cs.wrapbincol;
   cs.rewrap = brewrap;
   gs.rewrap = brewrap;
}

// circular link processing blocker.
// used only in a vertical walkFiles() processing run,
// e.g. parameters -dir dir1 -file .cpp -dir dir1 -file .hpp
// will run walkFiles() twice, each time resetting this:
KeyMap glblCircleMap;

// used with rename: map of simulated output filenames
KeyMap glblOutFileMap;

void cleanupTmpCmdData();
void shutdownAllGlobalData();

#ifndef USE_SFK_BASE
void resetStats()
{__
   void resetFileCounter();
   void resetFileSet();

   // reset command statistics:
   // copy global settings into local command settings
   memcpy(&cs, &gs, sizeof(cs));
   resetFileCounter();

   // reset command settings
   // cs.quiet = 0; // sfk1933 enable global use
   pszGlblSaveTo = 0;
   if (pszGlblSinceDir) { delete [] pszGlblSinceDir; pszGlblSinceDir=0; }

   // reset selected dirs and files
   resetFileSet();

   // reset all temporary command data
   cleanupTmpCmdData();

   // reset circular dependency blocker
   glblCircleMap.reset();
}
#endif // USE_SFK_BASE

CommandStats::CommandStats()
{
   memset(this, 0, sizeof(*this));
   reset();
}

void CommandStats::reset()
{
   memset(this, 0, sizeof(*this));
   wpat        =  1;
   runCmd      = str("");
   treeStopRC  = 19; // NOT 9
   subdirs     =  1;
   maxsub      =  0;
   utf8dec     =  0;
   wchardec    =  0; // experimental, NOT yet default
   usecirclemap=  1;
   wrapbincol  = 80; // default
   addsnaplf   = "\n";
   withrootdirs=  1;
   #ifdef _WIN32
   strcpy(szeol, "\r\n");
   #else
   strcpy(szeol, "\n");
   #endif
   toisodef    = '.';
   maxwebsize  = 100 * 1000000; // sfk196

   // sfk180: label -qtrim is default.
   mlquotes    = 'f';
   cs.curcmd[0] = '\0';

   cs.cweb     = 1;

   #ifdef _WIN32
   usecolor       =  1;
   usehelpcolor   =  1;
   #else
   usecolor       =  0;
   usehelpcolor   =  0;
   #endif

   outcconv       = 1;  // sfkwin only
   forcecconv     = 0;  // sfkwin only
   curport        = -1; // sfk197
   chan           = 1;
   chanserv       = 2;
   cliptries      = 6;

   keepchain      = 1; // sfk1990 new default

   if (cs.headers) {
      delete [] cs.headers;
      cs.headers = 0;
   }
}

bool CommandStats::stopTree(int nrc, bool *psilent)
{
   int lRC = 0;
   int nShellRC = 0;
   if (cs.stopfiletree) {
      if (psilent)
         *psilent = 1;
      return 1;
   }
   if (nrc >= treeStopRC) {
      if (!toldTreeStop) {
         toldTreeStop = 1;
         #ifndef USE_SFK_BASE
         int pinf(const char *pszFormat, ...);
         pinf("directory tree processing stopped by error.\n");
         #endif // USE_SFK_BASE
      }
      lRC = 1;
      // on stop, always map fatal rc to shell rc
      if (nrc >= 9) nShellRC = 9;   // error occurred, processing stopped.
      else          nShellRC = nrc; // should not happen
   } else {
      // map masked rc to shell rc
      if (cs.rcFromError) {
         if (nrc >= 7) nShellRC = 7;   // error occurred, processing continued.
         else          nShellRC = nrc; // any other code below 7
      }
   }
   // build maximum shell rc, if any
   if (nShellRC > nGlblShellRC)
      nGlblShellRC = nShellRC;
   // continue or stop tree processing
   return lRC ? 1 : 0;
}

bool CommandStats::showstat( )
{
   if (cs.nostat) return 0;
   if (cs.dostat) return 1;
   if (cs.quiet)  return 0;
   return 1;
}

bool CommandStats::withsub(int iLevel)
{
   if (!cs.subdirs) return 0; // -nosub set
   if (!cs.maxsub)  return 1; // default: process all
   if (iLevel <= cs.maxsub)
      return 1; // only up to given level
   return 0;
}

struct CommandPermamentStorage
{
public:
   CommandPermamentStorage ( );

   num  tailnsize;   // current file size of tail
   num  tailnpos;    // current read position of tail
   num  tailtime;    // current file modification time
   bool keeptmp;     // do not autodelete tmp files
   bool showtmp;     // verbosely list names of created tmpfiles
}
   cperm;

CommandPermamentStorage::CommandPermamentStorage() { memset(this, 0, sizeof(*this)); }

// sfk197 parse host:port, ~, ~port, ~:port.
// cs.curport stays unchanged if port is left out.
int csGetHostPort(char *psz)
{
   // ~ ~port ~:port
   if (*psz == '~') {
      strcopy(cs.curhost, "localhost");
      psz++;
      if (*psz == ':') psz++;
      if (isdigit(*psz)) {
         cs.curport = atoi(psz);
         if (cs.curport < 0 || cs.curport > 65535)
            return 9+perr("invalid port: %s", psz);
      }
      return 0;
   }

   // host host:port
   strcopy(cs.curhost, psz);
   char *pcol = strchr(cs.curhost, ':');
   if (pcol) {
      *pcol++ = '\0';
      cs.curport = atoi(pcol);
      if (cs.curport < 0 || cs.curport > 65535)
         return 9+perr("invalid port: %s", pcol);
   }
   return 0;
}

/*
   see also cs.curcmd. defined per source file,
   separate instances for submain, extmain.
*/

static bool bGlblCurCmdSet = 0;
static char szGlblCurCmd[50];



void initWildCards()
{
   #ifdef _WIN32
   char c = '*'; // windows wildcard default
   #else
   char c = '%'; // linux wildcard default
   #endif

   do {
      char *pszWC = getenv("SFK_CONFIG"); // wildstar
      if (!pszWC) break;
      pszWC = strstr(pszWC, "wildstar:");
      if (!pszWC) break;
      pszWC += strlen("wildstar:");
      if (!*pszWC) break;
      c = *pszWC;
   } while (0);
   glblWildChar = c;
}

bool isWildChar(char c) {
   // 1770: optional dual wildchar support with windows.
   //       was always the case with sfk linux.
   return (c == '*' || c == glblWildChar) ? 1: 0;
}
bool isWildStr(char *p) {
   if (strlen(p) != 1) return false;
   return isWildChar(p[0]);
}
bool isNotChar(char c) {
   #ifdef _WIN32
   // sfk windows: support dual not char
   return (c == '!' || c == glblNotChar) ? 1 : 0;
   #else
   // sfk linux: avoid possible conflicts with "!"
   return (c == glblNotChar) ? 1 : 0;
   #endif
}
bool isUniPathChar(char c) { // sfk183
   if (c == glblPathChar) return true;
   if (glblUPatMode==1 && c=='/') return true;
   return false;
}
void setLinuxSyntax(int iLevel) {
   glblUPatMode=1;
   glblNotChar  = ':';
   glblRunChar  = '#';
   if (iLevel >= 2) {
      glblWildChar = '%';
   }
}

int containsWildCards(char *pszName);
int mySetFileTime(char *pszFile, num nTime);
bool strbeg(char *psz, cchar *pstart);
bool stribeg(char *psz, cchar *pstart);
// strends  -> strEnds
// striends -> striEnds

void setTextColor(int n, bool bStdErr=0, bool bVerbose=0);
int execToHtml(int imode, int iaspect, char *plist, char *pszOutFile);
int installSFK(char *pszFolder, bool byes);
int reformatjson(char *pinbuf, char *poutbuf, char *poutattr, int ioutmax);
int execPhraser(char *pszAll, char *pszSrc, int iNumRec);
extern const char *szGlblPhraseData;

bool bGlblRandSeeded = 0;

// ========== lowest level printf redirect ============

#ifdef printf
 #undef printf
#endif
int sfkprintf(const char *pszFormat, ...);
#define printf sfkprintf

// ========== 64 bit abstraction layer begin ==========

#ifdef SFK_W64
extern "C"
{
int _fseeki64(FILE *stream, __int64 offset, int origin);
__int64 _ftelli64(FILE * _File);
}
#endif // SFK_W64

char *numtostr(num n, int nDigits, char *pszBuf, int nRadix)
{
   static char szBuf[100];
   if (!pszBuf)
        pszBuf = szBuf;

   #ifdef _WIN32
   if (nRadix == 10)
      sprintf(pszBuf, "%0*I64d", nDigits, n);
   else
      sprintf(pszBuf, "%0*I64X", nDigits, n);
   return pszBuf;
   #else
   if (nRadix == 10)
      sprintf(pszBuf, "%0*lld", nDigits, n);
   else
      sprintf(pszBuf, "%0*llX", nDigits, n);
   return pszBuf;
   #endif
}

char *numtoa_blank(num n, int nDigits)
{
   static char szBuf2[100];
   #ifdef _WIN32
   sprintf(szBuf2, "%*I64d", nDigits, n); // FIX: 1674
   return szBuf2;
   #else
   sprintf(szBuf2, "%*lld", nDigits, n);  // FIX: 1674
   return szBuf2;
   #endif
}

char *numtoa(num n, int nDigits, char *pszBuf) {
   return numtostr(n, nDigits, pszBuf, 10);
}

char *numtohex(num n, int nDigits, char *pszBuf) {
   return numtostr(n, nDigits, pszBuf, 0x10);
}

num atonum(char *psz)
{
   #ifdef _WIN32
   return _atoi64(psz);
   #else
   return atoll(psz);
   #endif
}

// atonum with support for decimal and 0x hex values
num myatonum(char *psz)
{
   if (!strncmp(psz, "0x", 2)) {
      #ifdef _MSC_VER
      return _strtoui64(psz+2, 0, 0x10);
      #else
      return strtoull(psz+2, 0, 0x10);
      #endif
   } else {
      return atonum(psz);
   }
}

mytime_t getSystemTime()
{
   static mytime_t stSysTime = 0;
   return mytime(&stSysTime);
}

#ifdef _WIN32
int mygetpos64(FILE *f, num &rpos, char *pszFile)
{
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   rpos = (num)npos1;
   return 0;
}
int mysetpos64(FILE *f, num pos, char *pszFile)
{
   fpos_t npos1 = (fpos_t)pos;
   if (fsetpos(f, &npos1))
      return 9+perr("setpos failed on %s\n", pszFile);
   return 0;
}
#else
int mygetpos64(FILE *f, num &rpos, char *pszFile)
{
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   #if defined(MAC_OS_X) || defined(SOLARIS)
   rpos = (num)npos1;
   #else
   rpos = (num)npos1.__pos;
   #endif
   return 0;
}
int mysetpos64(FILE *f, num pos, char *pszFile)
{
   // fetch "status" first
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   #if defined(MAC_OS_X) || defined(SOLARIS)
   npos1 = (fpos_t)pos;
   #else
   npos1.__pos = (__off_t)pos;
   #endif
   if (fsetpos(f, &npos1))
      return 9+perr("setpos failed on %s\n", pszFile);
   return 0;
}
#endif

// ========== 64 bit abstraction layer end ============

// - - - SFK primitive function library begin

static char szErrBuf[MAX_LINE_LEN+10];
static char szPreErrBuf[MAX_LINE_LEN+10];

cchar *sfkLastError()
{
   if (bErrBufSet)
      return szErrBuf;
   else
      return "";
}

// optional reroute of error messages
int (*pGlblSFKStatusCallBack)(int nMsgType, char *pmsg) = 0;

int errPauseOrEcho( )
{
   if (cs.echoonerr) {
      if (cs.argc > 0 && cs.argv != 0) {
         fprintf(stderr, "[cmd]: ");
         for (int i=0; i<cs.argc; i++)
            fprintf(stderr, "%s ", cs.argv[i]);
         fprintf(stderr, "\n");
      }
      else {
         printf("cmd  : [none]\n");
      }
   }

   if (bGlblPauseOnError) {
      printf("Press ENTER to continue.\n");
      while (getchar() != '\n');
   }
 
   return 0;
}

/*
   the latest gcc compilers produce sick warnings

      -Wformat-overflow
      -Wformat-truncation
      -Wstringop-overflow

   which have absolutely no use.

   to avoid adding tons of -Wformat-overflow=0 etc.
   to every compile command, sfk uses these redirects.

   they are not strictly needed with Windows VC,
   but used also there for a unified code path.
*/
int mysnprintf(char *pbuf, size_t nmax, const char *pmask, ...)
{
   va_list argList;
   va_start(argList, pmask);
   #ifdef _WIN32
   return _vsnprintf(pbuf, nmax, pmask, argList);
   #else
   return ::vsnprintf(pbuf, nmax, pmask, argList);
   #endif
}
int mysprintf(char *pbuf, const char *pmask, ...)
{
   va_list argList;
   va_start(argList, pmask);
   return ::vsprintf(pbuf, pmask, argList);
}
void mystrncpy(char *pdst, const char *psrc, size_t nmaxdst)
{
   int nsrc = strlen(psrc);
   if (nsrc+1 >= nmaxdst)
       nsrc = nmaxdst-1;
   memcpy(pdst, psrc, nsrc);
   pdst[nsrc] = '\0';
}

int perr(const char *pszFormat, ...)
{
   if (cs.noerr) return 0;

   static bool bWithinPErr = 0;

   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szErrBuf, sizeof(szErrBuf)-10, pszFormat, argList);
   szErrBuf[sizeof(szErrBuf)-10] = '\0';

   if (cs.errtotext && !bWithinPErr) {
      bWithinPErr = 1;
      removeCRLF(szErrBuf);
      chain.print('e', 0, "error: %s\n", szErrBuf);
      bWithinPErr = 0;
      // error is "catched" so allow no pause etc.
      return 0;
   }
   else
   if (pGlblSFKStatusCallBack) {
      // output to callback
      removeCRLF(szErrBuf);
      pGlblSFKStatusCallBack(1, szErrBuf);
   } else {
      // output to terminal
      if (!strchr(szErrBuf, '\n'))
         strcat(szErrBuf, "\n");
      // sfk197 prefix error by current command
      info.clear();
      setTextColor(nGlblErrColor, 1); // on stderr
      #ifdef USE_SFK_BASE
      fprintf(stderr, "error: %s", szErrBuf);
      #else
      fprintf(stderr, "error: sfk %s: %s", cs.curcmd, szErrBuf);
      #endif
      setTextColor(-1, 1);
   }

   mtkerr(("%s", szErrBuf));

   bErrBufSet = 1;
   nGlblErrors++;

   errPauseOrEcho();

   return 0;
}

// file not found unified error with filename check
int pferr(const char *pszFile, const char *pszFormat, ...)
{
   char szBuf[SFK_MAX_PATH+100];
   char aCode[256];

   if (cs.noerr) return 0;

   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPreErrBuf, sizeof(szPreErrBuf)-10, pszFormat, argList);
   szPreErrBuf[sizeof(szPreErrBuf)-10] = '\0';

   if (strBegins(szPreErrBuf, "[warn] "))
      pwarn("%s", szPreErrBuf+7);
   else
      perr("%s", szPreErrBuf);

   memset(aCode, 0, sizeof(aCode));
   bool bvalid=1;
   for (uchar *p=(uchar*)pszFile; *p; p++) {
      aCode[*p]=1;
      if (*p < 0x20)
         bvalid=0;
   }
   if (aCode['\r'] || aCode['\n']) {
      pinf("filename contains CR or LF line end characters:\n");
      pinf("  %s\n", dataAsTrace((void*)pszFile, strlen(pszFile), szBuf, sizeof(szBuf)));
      pinf("  use echo -pure or +xed \"/[eol]//\" in previous commands.\n");
   } else if (!bvalid) {
      pinf("filename contains invalid characters. look for {nn} codes in the following dump:\n");
      pinf("  %s\n", dataAsTrace((void*)pszFile, strlen(pszFile), szBuf, sizeof(szBuf)));
   }

   return 0;
}

int pbad(char *pszCmd, char *pszParm)
{
   return perr("unexpected parameter \"%s\". (sfk %s)\n", pszParm, pszCmd);
}

int pcon(char *pszCmd, char *pszParm)
{
   perr("conflicting input from previous command and parameter: %s", pszParm);
   pinf("use \"+then %s\" to ignore command chain input text.\n", pszCmd);
   return 0;
}

int pwarn(const char *pszFormat, ...)
{
   if (cs.nowarn) return 0;
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szErrBuf, sizeof(szErrBuf)-10, pszFormat, argList);
   szErrBuf[sizeof(szErrBuf)-10] = '\0';

   if (pGlblSFKStatusCallBack) {
      // output to callback
      removeCRLF(szErrBuf);
      char *pbuf = szErrBuf;
      if (!strncmp(pbuf, "[nopre] ", 8))
         pbuf += 8;
      pGlblSFKStatusCallBack(2, pbuf);
   } else {
      // output to terminal
      if (!strchr(szErrBuf, '\n'))
         strcat(szErrBuf, "\n");
      info.clear();
      setTextColor(nGlblWarnColor, 1);
      char *psz = szErrBuf;
      // sfk197 prefix warn by current command
      if (!strncmp(psz, "[nopre] ", 8))
         fprintf(stderr, "sfk %s: %s", cs.curcmd, psz+8);
      else
         fprintf(stderr, "warn : sfk %s: %s", cs.curcmd, psz);
      setTextColor(-1, 1);
   }

   mtkwarn(("%s", szErrBuf));
   nGlblWarnings++;

   return 0;
}

int pinf(const char *pszFormat, ...)
{
   if (cs.nonotes) return 0;

   // does NOT use szErrBuf to allow access to last
   // error message through sfkLastError().

   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBuf1, sizeof(szPrintBuf1)-10, pszFormat, argList);
   szPrintBuf1[sizeof(szPrintBuf1)-10] = '\0';

   if (pGlblSFKStatusCallBack) {
      // output to callback
      removeCRLF(szPrintBuf1);
      pGlblSFKStatusCallBack(3, szPrintBuf1);
   } else {
      // output to terminal
      info.clear();
      setTextColor(nGlblTimeColor, 1);
      char *psz = szPrintBuf1;
      if (!strncmp(psz, "[nopre] ", 8))
         fprintf(stderr, "%s", psz+8);
      else
         fprintf(stderr, "note : %s", psz);
      setTextColor(-1, 1);
   }

   mtklog(("note: %s", szPrintBuf1));

   return 0;
}

int tellCannotWriteBatchFile(char *pszName)
{
   perr("cannot write batch file: %s\n", szRefNameBuf);
   #ifndef _WIN32
   pinf("batch files are put into the same folder as the sfk binary.\n");
   pinf("try placing sfk in a writeable folder listed first in the PATH,\n");
   pinf("for example: mkdir ~/bin; cp sfk ~/bin; export PATH=~/bin:$PATH\n");
   pinf("or try sudo, if you are the only user of this machine.\n");
   #endif
   return 0;
}

char *mystrerr(int iOptCode=-1)
{
   static char szBuf[100];
 
   szBuf[0] = '\0';

   #ifdef _WIN32

   DWORD nerr = (iOptCode >= 0) ? iOptCode : GetLastError();

   LPVOID lpMsgBuf = 0;

   FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, nerr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        // MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&lpMsgBuf,
        0, NULL );

   if (lpMsgBuf) {
      removeCRLF((char*)lpMsgBuf);
      snprintf(szBuf, sizeof(szBuf)-10, "%d,%s", nerr, (char*)lpMsgBuf);
   }

   LocalFree(lpMsgBuf);

   #else

   int nerr = (iOptCode >= 0) ? iOptCode : errno;

   char *psz = strerror(nerr);
   snprintf(szBuf, sizeof(szBuf)-10, "%d,%s", nerr, psz);

   #endif
 
   return szBuf;
}

void perrinfo(const char *pszContext)
{
   // general reasons first
   if (!strcmp(pszContext, "fwrite")) {
      pinf("the target volume may have no space left on the device.\n");
      pinf("if the target is a network drive, a file size limit may apply.\n");
   }

   // then dump system infos, if any
   #ifdef _WIN32

   #ifdef WINFULL
   #ifdef USE_SFK_BASE
   int  nerr1 = 0;
   #else
   int  nerr1 = errno;
   #endif
   DWORD nerr2 = GetLastError();
   LPVOID lpMsgBuf = 0;
   FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, nerr2,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
   if (lpMsgBuf) removeCRLF((char*)lpMsgBuf);
   info.clear(); // in case no perr was done
   setTextColor(nGlblWarnColor);
   printf("cerno: %d,%s\n", nerr1, strerror(nerr1));
   printf("werno: %u,%s\n", nerr2, lpMsgBuf ? lpMsgBuf : "");
   setTextColor(-1);
   LocalFree(lpMsgBuf);
   #endif

   #else

   int  nerr1 = errno;
   info.clear(); // in case no perr was done
   setTextColor(nGlblWarnColor);
   printf("cerno: %d,%s\n", nerr1, strerror(nerr1));
   setTextColor(-1);

   #endif
}

bool bGlblSysErrOccured = 0;

int esys(const char *pszContext, const char *pszFormat, ...)
{
   bGlblSysErrOccured = 1;

   // just like perr:
   if (cs.noerr) return 0;
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szErrBuf, sizeof(szErrBuf)-10, pszFormat, argList);
   szErrBuf[sizeof(szErrBuf)-10] = '\0';
   if (!strchr(szErrBuf, '\n'))
      strcat(szErrBuf, "\n");
   info.clear();
   setTextColor(nGlblErrColor, 1); // on stderr
   fprintf(stderr, "error: %s", szErrBuf);
   setTextColor(-1, 1);
   mtkerr(("%s", szErrBuf));
   bErrBufSet = 1;
   nGlblErrors++;

   // optionally extend output by runtime info:
   if (bGlblSysErrDetail)
      perrinfo(pszContext);

   errPauseOrEcho();

   return 0;
}

char *ownIPList(int &rhowmany, uint nOptPort, const char *psep, int nmode);

#if (defined(WITH_TCP) || defined(VFILENET) || defined(DV_TCP))

bool bGlblTCPInitialized = 0;

int prepareTCP()
{
   if (!bGlblTCPInitialized)
   {
      bGlblTCPInitialized = 1;

      #ifdef _WIN32
      WORD wVersionRequested = MAKEWORD(1,1);
      WSADATA wsaData;
      if (WSAStartup(wVersionRequested, &wsaData)!=0)
         return 9+perr("WSAStartup failed\n");
      #endif
   }
   return 0;
}

void shutdownTCP()
{
   if (!bGlblTCPInitialized)
      return;

   bGlblTCPInitialized = 0;

   #ifdef _WIN32
   WSACleanup(); // sfk1840 central, on process exit
   #endif
}

// all closesocket calls are redirected to:
void myclosesocket(SOCKET hsock, bool bread, bool bwrite)
{
   int nmode = 0;

   if (bread  && !bwrite)
      nmode = SHUT_RD;     // no more receptions
   else
   if (!bread && bwrite)
      nmode = SHUT_WR;     // no more transmissions
   else
   if (bread  && bwrite)
      nmode = SHUT_RDWR;   // no more transfers at all

   // signal the tcp stack that transmission stops,
   // so the receiver side may receive remaining data.
   shutdown(hsock, nmode);

   #ifdef _WIN32
   closesocket(hsock);
   #else
   close(hsock);
   #endif
}

#endif // WITH_TCP or VFILENET

// just close on a socket is not enough.
// myclosesocket also does the shutdown().
#define closesocket myclosesocket

num getCurrentTime()
{
   #ifdef _WIN32
   return (num)GetTickCount();
   #else
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return ((num)tv.tv_sec) * 1000 + ((num)tv.tv_usec) / 1000;
   #endif
}

// returns high resolution timer ticks, if available
num getCurrentTicks()
{
   #ifdef _WIN32
   LARGE_INTEGER val1;
   QueryPerformanceCounter(&val1);
   return val1.QuadPart;
   #else
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return ((num)tv.tv_sec) * 1000 + ((num)tv.tv_usec) / 1000;
   #endif
}

num msecFromTicks(num nTicksDiff)
{
   #ifdef _WIN32
   LARGE_INTEGER val1;
   QueryPerformanceFrequency(&val1);
   num nMicroSecPeriod = val1.QuadPart;

   if (nMicroSecPeriod <= 0)
      return 1;

   num nMSecDiff = nTicksDiff * 1000 / nMicroSecPeriod;

   if (nMSecDiff <= 0)
      return 1;

   return nMSecDiff;
   #else
   return nTicksDiff;
   #endif
}

char *myvtext(const char *pszFormat, ...)
{
   static char szBuf[4096];
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szBuf, sizeof(szBuf)-10, pszFormat, argList);
   szBuf[sizeof(szBuf)-10] = '\0';
   return szBuf;
}

// copies a maximum of nMaxDst MINUS ONE chars,
// AND adds a zero terminator at pszDst (within nMaxDst range!).
// to use this like strncpy, always add +1 to nMaxDst.
// NOTE: if nMaxDst == 0, NO zero terminator is added.
void mystrcopy(char *pszDst, cchar *pszSrc, int nMaxDst) {
   if (nMaxDst < 2) {
      if (nMaxDst >= 1)
         pszDst[0] = '\0';
      return;
   }
   int nLen = strlen(pszSrc);
   if (nLen > nMaxDst-1)
      nLen = nMaxDst-1;
   memcpy(pszDst, pszSrc, nLen);
   pszDst[nLen] = '\0';
}
#define strcopy(dst,src) mystrcopy(dst,src,sizeof(dst)-10)

int mystrwlen(const ushort *psz) {
   int ilen=0;
   while (psz[ilen])
      ilen++;
   return ilen;
}

void mystrwcopy(ushort *pszDst, const ushort *pszSrc, int nMaxChr) {
   if (nMaxChr < 2) {
      if (nMaxChr >= 1)
         pszDst[0] = '\0';
      return;
   }
   int nChr = mystrwlen(pszSrc);
   if (nChr > nMaxChr-1)
      nChr = nMaxChr-1;
   memcpy(pszDst, pszSrc, nChr*2);
   pszDst[nChr] = '\0';
}
#define strwcopy(dst,src) mystrwcopy(dst,src,(int)(sizeof(dst)-10)/2)

ushort *mystrwdup(ushort *psrc,int *pnchars=0)
{
   int nchr=mystrwlen(psrc);
   ushort *pres=new ushort[nchr+2]; // with tolerance
   if (!pres) return 0;
   memset(pres,0,(nchr+2)*2);
   memcpy(pres,psrc,(nchr+1)*2); // with term
   if (pnchars) *pnchars=nchr;
   return pres;
}

uchar *mymemdup(char *psz, int nlen)
{
   uchar *p = new uchar[nlen+2];
   if (!p) return p;
   memcpy(p, psz, nlen);
   p[nlen] = '\0'; // tolerance
   return p;
}

// remove blanks from right side of a string
void myrtrim(char *pszBuf) {
   int nlen = strlen(pszBuf);
   while (nlen > 0 && pszBuf[nlen-1] == ' ') {
      pszBuf[nlen-1] = '\0';
      nlen--;
   }
}

void myrtrimany(char *pszBuf, cchar *pfilt) {
   int nlen = strlen(pszBuf);
   while (nlen > 0 && strchr(pfilt, pszBuf[nlen-1])) {
      pszBuf[nlen-1] = '\0';
      nlen--;
   }
}

void skipUntil(char **pp, cchar *pdelim) {
   char *p = *pp;
   while (*p && !strchr(pdelim, *p))
      p++;
   *pp = p;
}

void skipOver(char **pp, cchar *pdelim) {
   char *p = *pp;
   while (*p && strchr(pdelim, *p))
      p++;
   *pp = p;
}

bool isws(char c,bool bext=0)
{
   if (c == ' ') return 1;
   if (c == '\t') return 1;
   if (bext) {
      if (c=='\r' || c=='\n')
         return 1;
   }
   return 0;
}

bool iseol(char c) {
   if (c == '\r') return 1;
   if (c == '\n') return 1;
   return 0;
}

void skipToWhite(char **pp) { skipUntil(pp, " \t\r\n"); }
void skipWhite(char **pp)   { skipOver(pp, " \t\r\n");  }

int nextLine(char **pp) {
   char *p = *pp;
   while (*p!=0 && *p!='\r' && *p!='\n')
      p++;
   if (*p!='\r' && *p!='\n')
      return 1;
   while (*p!=0 && (*p=='\r' || *p=='\n'))
      *p++ = '\0';
   *pp = p;
   return 0;
}

void swapchars(char *p, num nlen)
{
   if (nlen < 2)
      return;

   num nhalf = nlen / 2;
   num imax  = nlen - 1;

   for (num i=0; i<nhalf; i++)
   {
      char c = p[i];
      p[i] = p[imax-i];
      p[imax-i] = c;
   }
}

bool alldigits(char *psz) {
   for (; *psz; psz++)
      if (!isdigit(*psz))
         return 0;
   return 1;
}

struct tm *mylocaltime(mytime_t *ptime)
{
   #ifdef SFK_W64
   return _localtime64(ptime);
   #else
   return localtime(ptime);
   #endif
}

struct tm *mygmtime(mytime_t *ptime)
{
   #ifdef SFK_W64
   return _gmtime64(ptime);
   #else
   return gmtime(ptime);
   #endif
}

bool myisxdigit(char c) {
   if (c >= '0' && c <= '9') return 1;
   if (c >= 'a' && c <= 'f') return 1;
   if (c >= 'A' && c <= 'F') return 1;
   return 0;
}

int getTwoDigitHex(char *psz)
{
   char szHex[10];

   if (!*psz) return -1;
   szHex[0] = tolower(*psz++);
   if (!myisxdigit(szHex[0])) return -1;

   if (!*psz) return -1;
   szHex[1] = tolower(*psz++);
   if (!myisxdigit(szHex[1])) return -1;

   szHex[2] = '\0';

   return (int)strtoul(szHex,0,0x10);
}

int getThreeDigitDec(char *psz)
{
   char szDec[10];

   if (!*psz) return -1;
   szDec[0] = tolower(*psz++);
   if (!isdigit(szDec[0])) return -1;

   if (!*psz) return -1;
   szDec[1] = tolower(*psz++);
   if (!isdigit(szDec[1])) return -1;

   if (!*psz) return -1;
   szDec[2] = tolower(*psz++);
   if (!isdigit(szDec[2])) return -1;

   szDec[3] = '\0';

   return (int)strtoul(szDec,0,10);
}

void doSleep(int nmsec)
{
   #ifdef _WIN32
   Sleep(nmsec);
   #else
   // sleep(1);
   const timespec ts = { nmsec / 1000, nmsec % 1000 * 1000000 };
   nanosleep(&ts, NULL);
   #endif
}

void doYield( )
{
   #ifdef _WIN32
    #if (defined(_MSC_VER) && (_MSC_VER < 1900))
     // fix compile error with sfk-winxp
     doSleep(1);
    #else
     SwitchToThread();
    #endif
   #else
   // yield();
   ::usleep(50);
   #endif
}

char *mystrrstr(char *psrc, cchar *ppat)
{
   if (!psrc || !ppat) return 0;

   char c = *ppat;
   int nsrclen = strlen(psrc);
   int npatlen = strlen(ppat);
   if (npatlen > nsrclen) return 0;

   char *pcur = psrc + nsrclen - npatlen;
   while (pcur >= psrc) {
      if (*pcur == c && !strncmp(pcur, ppat, npatlen))
         return pcur;
      pcur--;
   }
   return 0;
}

char *mystrristr(char *psrc, cchar *ppat)
{
   if (!psrc || !ppat) return 0;

   int nsrclen = strlen(psrc);
   int npatlen = strlen(ppat);
   if (npatlen > nsrclen) return 0;

   char *pcur = psrc + nsrclen - npatlen;
   while (pcur >= psrc) {
      if (!mystrnicmp(pcur, ppat, npatlen))
         return pcur;
      pcur--;
   }
   return 0;
}

int setWriteEnabled(char *pszFile)
{
   #ifdef _WIN32

   BOOL bok = 0;

   #ifdef WINFULL
   WIN32_FILE_ATTRIBUTE_DATA oinf;
   bok = GetFileAttributesEx(pszFile, GetFileExInfoStandard, &oinf);
   if (!bok) return 9;

   oinf.dwFileAttributes &= (0xFFFFFFFFUL ^ FILE_ATTRIBUTE_READONLY);
   bok = SetFileAttributes(pszFile, oinf.dwFileAttributes);
   #else
   uint nattrib = GetFileAttributesA(pszFile);
   nattrib &= (0xFFFFFFFFUL ^ FILE_ATTRIBUTE_READONLY);
   SetFileAttributes(pszFile, nattrib);
   #endif

   if (!bok) return 9;

   #else

   struct stat64 buf;
   if (stat64(pszFile, &buf)) return 9;

   mode_t nmode = buf.st_mode | _S_IWRITE;
   if (chmod(pszFile, nmode)) return 9;
 
   #endif

   return 0;
}

FILE *pGlblOpenWriteFile   = 0;
char  szGlblOpenWriteName[MAX_LINE_LEN+10];

void beginFileWrite(char *pszName, FILE *p) {
   pGlblOpenWriteFile = p;
   strcopy(szGlblOpenWriteName, pszName);
}
void endFileWrite() {
   pGlblOpenWriteFile = 0;
   szGlblOpenWriteName[0] = 0;
}
void checkFileWrite() {
   if (pGlblOpenWriteFile)
      perr("unexpected: file left open for write: %s\n", szGlblOpenWriteName);
}
void cleanupFileWrite() {
   if (pGlblOpenWriteFile) {
      // close and remove incomplete (trash) file.
      // may not work here as this is called during interrupt.
      fclose(pGlblOpenWriteFile);
      remove(szGlblOpenWriteName);
   }
}
FILE *myfopen(char *pszName, cchar *pszMode) {
   FILE *f = fopen(pszName, pszMode);
   if (!f && !strcmp(pszMode, "wb") && fileExists(pszName)) {
      // file is probably write protected
      if (setWriteEnabled(pszName))
         return 0;
      // retry on write-enabled file
      f = fopen(pszName, pszMode);
   }
   if (f) {
      if (pGlblOpenWriteFile)
         pwarn("open for write on %s not registered\n", pszName);
      else
         beginFileWrite(pszName, f);
   }
   return f;
}
void myfclose(FILE *f) {
   if (f == pGlblOpenWriteFile)
      endFileWrite();
   fclose(f);
}

// FIX for Windows 60 MB I/O Bug: Windows XP fails to read blocks
// larger than 60 MByte, therefore use myfread instead of fread:

// large block read incl. optional info update and checksum building
size_t myfread(uchar *pBuf, size_t nBufSize, FILE *fin, num nMax, num nCur, SFKMD5 *pmd5)
{
   size_t nOffset  = 0;
   size_t nRemain  = nBufSize;
   size_t nReadSub = 0;
 
   while ((nRemain > 0) && !bGlblEscape)
   {
      size_t nBlock = SFK_IO_BLOCK_SIZE;
      if (nBlock > nRemain) nBlock = nRemain;

      #ifdef _WIN32
      // sfk1972 fix Visual 2015 stdin fread causes random joined lines.
      if (fin != stdin)
         nReadSub = fread(pBuf+nOffset, 1, nBlock, fin); // safe
      else
      if (!ReadFile(hGlblStdIn, pBuf+nOffset, nBlock, &nGlblStdRead, 0))
         { nReadSub = 0; } else { nReadSub = (size_t)nGlblStdRead; }
      #else
      nReadSub = fread(pBuf+nOffset, 1, nBlock, fin); // safe
      #endif

      if (nReadSub <= 0)
         break;

      nOffset += nReadSub;
      nRemain -= nReadSub;

      if (nMax > 0)
         info.setProgress(nMax, nCur+nOffset, "bytes");
   }

   if (nOffset > 0 && pmd5 != 0)
      pmd5->update(pBuf, nOffset);
 
   return nOffset;
}

// large block write incl. optional info update and checksum building
size_t myfwrite(uchar *pBuf, size_t nBytes, FILE *fout, num nMax, num nCur, SFKMD5 *pmd5)
{
   size_t nOffset = 0;
   size_t nRemain = nBytes;

   while ((nRemain > 0) && !bGlblEscape)
   {
      size_t nBlock = SFK_IO_BLOCK_SIZE;
      if (nBlock > nRemain) nBlock = nRemain;

      size_t nWriteSub = fwrite(pBuf+nOffset, 1, nBlock, fout);

      // mtklog(("myfwrite: %d = fwrite(%d)",(int)nWriteSub,(int)nBlock));

      if (nWriteSub != nBlock)
         return nOffset+nWriteSub; // return no. of bytes actually written

      nOffset += nWriteSub;
      nRemain -= nWriteSub;

      if (nMax > 0)
         info.setProgress(nMax, nCur+nOffset, "bytes");
   }

   if (nOffset > 0 && pmd5 != 0)
      pmd5->update(pBuf, nOffset);

   return nOffset;
}

#ifdef _WIN32
   #define mypopen _popen
   #define mypclose _pclose
#else
   #define mypopen popen
   #define mypclose pclose
#endif

#ifdef _WIN32
void timetToFileTime(num ntimet, FILETIME *pft) // sfk1933 full 64 bits
{
   #if 1

   ntimet *= 10000000LL;
   ntimet += 116444736000000000LL;

   LONGLONG ll = (LONGLONG)ntimet;

   pft->dwLowDateTime  = (DWORD)ll;
   pft->dwHighDateTime = (DWORD)(ll>>32);

   #else

   time_t t = (time_t)ntimet; // time32 deactivated with sfk1933

   LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000LL;

   pft->dwLowDateTime  = (DWORD)ll;
   pft->dwHighDateTime = (DWORD)(ll>>32);

   #endif
}

num fileTimeToTimeT(num nwft)
{
   nwft -= 116444736000000000LL;
   nwft /= 10000000;
   // sfk197: never return negative times
   if (nwft < 0) nwft = 0;
   return nwft;
}

num fileTimeToTimeT(FILETIME *pft)
{
   num nwft =     (((num)pft->dwHighDateTime) << 32)
               |  (((num)pft->dwLowDateTime));
   nwft -= 116444736000000000LL;
   nwft /= 10000000;
   // sfk197: never return negative times
   if (nwft < 0) nwft = 0;
   return nwft;
}
#endif

size_t safefread(void *pBuf, size_t nBlockSize, size_t nBufSize, FILE *fin)
   { return myfread((uchar*)pBuf, nBufSize, fin); }

size_t safefwrite(const void *pBuf, size_t nBlockSize, size_t nBufSize, FILE *fin)
   { return myfwrite((uchar*)pBuf, nBufSize, fin); }

// FROM HERE ON, ALL fread() and fwrite() calls are MAPPED to SAFE versions
// to work around Windows runtime bugs (60 MB I/O bug, stdin joined lines etc.)

#define fread  safefread
#define fwrite safefwrite

bool cmpchr(char c1, char cmsk, bool bcase, bool besc)
{
   if (!besc && cmsk == '?') return 1;
   if (!bcase) c1   = sfktolower(c1);
   if (!bcase) cmsk = sfktolower(cmsk);
   if (c1 == cmsk) return 1;
   return 0;
}

void trimLine(char *psz, int iMode, int *pChg=0)
{
   char *pleft  = psz;
   char *prite  = psz + strlen(psz);
   char *prite2 = prite;
   int   iChg   = 0;

   if (iMode & 1)
   {
      while (*pleft == ' ' || *pleft == '\t') {
         pleft++;
         iChg++;
      }
   }

   if (iMode & 2)
   {
      while (prite > pleft && (prite[-1] == ' ' || prite[-1] == '\t')) {
         prite--;
         iChg++;
      }
   }

   int ilen = prite - pleft;

   if (pleft != psz)
   {
      // move and terminate
      for (int i=0; i<ilen; i++)
         psz[i] = pleft[i];
      psz[ilen] = '\0';
      if (pChg) *pChg = iChg;
   }
   else if (prite != prite2)
   {
      // just terminate
      *prite = '\0';
      if (pChg) *pChg = iChg;
   }
}

uchar unicodeToSimpleIso(uint ucode)
{
   switch (ucode)
   {
      // normalize stupid apostrophes
      case 0x2018: ucode = 0x27; break;
      case 0x2019: ucode = 0x27; break;
      case 0x0000: ucode = cs.toisodef; break;
      default:
         // replace all non-8bit by default
         if (ucode >= 0x0100)
            ucode = cs.toisodef;
            break;
   }
   return (uchar)ucode;
}

void utf8ToSimpleIso(char *psz, int *pChg=0)
{
   UTF8Codec utf(psz);
   char *pDstCur = psz;
   char *pDstMax = psz+strlen(psz);
   uint ucode = 0;
   while (pDstCur<pDstMax && utf.hasChar()!=0)
   {
      *pDstCur++ = (char)unicodeToSimpleIso(utf.nextChar());
   }
   if (pDstCur<pDstMax) {
      *pDstCur = '\0';
      if (pChg) (*pChg)++;
   }
}

void mystrcatf(char *pOut, int nOutMax, cchar *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBufMap, sizeof(szPrintBufMap)-10, pszFormat, argList);
   szPrintBufMap[sizeof(szPrintBufMap)-10] = '\0';
   char *psz = szPrintBufMap;

   if (nOutMax == 0) nOutMax = MAX_LINE_LEN;

   int nlen1 = strlen(pOut);
   int nrem1 = (nOutMax - nlen1) - 1; // including term.
   int nlen2 = strlen(psz);
   if (nlen2 > nrem1) nlen2 = nrem1;
   if (nlen2 > 0) {
      memcpy(pOut+nlen1, psz, nlen2);
      *(pOut+nlen1+nlen2) = '\0';
   }
}

// without zero termination, only for short fixed-size strings
void mystrplot(char *pOut, int iMaxOut, cchar *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBufMap, sizeof(szPrintBufMap)-10, pszFormat, argList);
   szPrintBufMap[sizeof(szPrintBufMap)-10] = '\0';
   char *psz = szPrintBufMap;

   int iCopy = strlen(psz);
   if (iCopy > iMaxOut)
      iCopy = iMaxOut;
 
   memcpy(pOut, psz, iCopy);
}

char myrchar(char *psz)
{
   int ilen = strlen(psz);
   if (ilen > 0)
      return psz[ilen-1];
   return 0;
}

void printHtml(char *pszText, int iTextLen)
{
   for (int i=0; i<iTextLen; i++) {
      char c = pszText[i];
      switch (c) {
         case '>': printf("&gt;"); break;
         case '<': printf("&lt;"); break;
         case '&': printf("&amp;"); break;
         default : putchar(c); break;
      }
   }
}

// simple string matching with wildcard support

enum eMatchStr {
   eMatchCase     = 1,
   eMatchLiteral  = 2,
   eMatchHead     = 4,
   eMatchTail     = 8
};

char peekpatchr(char *p, int &rEscaped)
{
   rEscaped = 0;
   char c  = *p++;
   if (!cs.spat || c != '\\') // spat.3 peek 2
      return c;
   char c2 = *p;
   switch (c2) {
      case 't' : rEscaped=1; c='\t'; break;
      case 'q' : rEscaped=1; c='"'; break;
      case '\\':
      case '*' :
      case '?' : rEscaped=1; c=c2;   break;
      case 'x' :
         if (p[1] && p[2]) {
            rEscaped=3;
            c = (char)getTwoDigitHex(p+1);
         }
         break;
   }
   return c;
}

bool mystrhit(char *pszStr, char *pszPat, bool bCase, int *pOutHitIndex)
{
   if (bCase) {
      char *psz = strstr(pszStr, pszPat);
      if (psz) {
         if (pOutHitIndex) *pOutHitIndex = (int)(psz-pszStr);
         return true;
      } else {
         if (pOutHitIndex) *pOutHitIndex = -1;
         return false;
      }
   } else {
      if (mystrstrip(pszStr, pszPat, pOutHitIndex))
         return true;
      else
         return false;
   }
}

bool matchstr(char *pszHay, char *pszPat, int nFlags, int &rfirsthit, int &rhitlen)
{
   if (cs.debug)
      printf("match: enter matchstr \"%s\" \"%s\" flags %u\n",pszHay,pszPat,nFlags);

   bool bCase = ( nFlags & 1) ? 1 : 0;
   bool bHead = ( nFlags & 4) ? 1 : 0;
   bool bTail = ( nFlags & 8) ? 1 : 0;

   int nhaylen = strlen(pszHay);
   int ibase   = 0;
   int ifirst  = -1;     // pos'n of first matching char
   int ilast   = -1;     // pos'n of last matching char
   bool bmatch  = 0;
   int nesc    = 0;      // escapes not (0), or 1 or 3 chars

   if (!cs.wpat) {
      // no wildcard interpretation
      int npatlen = strlen(pszPat);
      if (bTail) {
         // check for end-of-line match
         if (nhaylen < npatlen) return 0;
         char *pend = pszHay+nhaylen-npatlen;
         if (!mystrncmp(pend,pszPat,npatlen,bCase)) {
            if (cs.debug)
               printf("match:  direct at %d len %d\n",nhaylen-npatlen,npatlen);
            rfirsthit = nhaylen-npatlen;
            rhitlen   = npatlen;
            return 1;
         }
         return 0;
      } else {
         // check for anywhere or start-of-line match
         int ihit = 0;
         bool brc = mystrhit(pszHay, pszPat, bCase, &ihit);
         if (!brc) return 0;
         if (bHead && ihit != 0) return 0;
         rfirsthit = ihit;
         rhitlen   = npatlen;
         if (cs.debug)
            printf("match:  direct at %d len %d\n",ihit,npatlen);
         rfirsthit = ihit;
         rhitlen   = npatlen;
         return 1;
      }
   }

   do
   {
      char *ppat = pszPat;       // pattern read cursor
      char *phay = pszHay+ibase; // haystack read cursor
 
      bool biskip = !bHead; // initial skip, add * at start
      ifirst = -1;     // pos'n of first matching char
      ilast  = -1;     // pos'n of last matching char
 
      bmatch=0;
      while (true)
      {
         if (!*ppat) {
            bmatch = 1;
            break;
         }

         char cpat  = 0;
         char cpat2 = 0;
         bool bdowc = cs.wpat; // do the wild char, or not
 
         if (biskip) {
            biskip = 0;
            cpat   = '*';
            if (cs.debug)
               printf("match:  process %c index %d \"%s\"\n", cpat, ppat-pszPat, ppat);
         } else {
            cpat  = *ppat++;
            cpat2 = *ppat;
            if (cs.debug)
               printf("match:  process %c index %d \"%s\"\n", cpat, ppat-pszPat-1, ppat-1);
         }
 
         // remapping of \\ \n \t \* \?
         if (cs.spat && cpat == '\\')  // spat.4 matchstr 22
         {
            switch (cpat2) {
               case '\\': ppat++; break;
               case 't' : ppat++; cpat = '\t'; break;
               case 'q' : ppat++; cpat = '"';  break;
               case '*' : ppat++; cpat = '*'; bdowc = 0; break;
               case '?' : ppat++; cpat = '?'; bdowc = 0; break;
               case 'x' :
                  if (ppat[1] && ppat[2]) {
                     cpat = (char)getTwoDigitHex(ppat+1);
                     ppat += 3;
                     bdowc = 0;
                  }
                  break;
            }
         }
 
         if (bdowc && cpat == '*')
         {
            // seek forward over *
            char cnext = peekpatchr(ppat, nesc);
            bool enext = (nesc > 0) ? 1 : 0;
            if (!cnext) { bmatch=1; break; }
            // **?
            if (!nesc && cnext == '*') continue;
            // isolate next non-* part
            int isrc=0,idst=0;
            while (idst<MAX_MATCH_BUF) {
               char csub = peekpatchr(ppat+isrc, nesc);
               if (nesc)
                  isrc += 1+nesc; // fetched \* or \xnn
               else {
                  if (!csub || csub == '*') break; // NO isrc increment on *
                  isrc++;
               }
               szMatchBuf[idst  ] = csub;
               szMatchEsc[idst++] = (bool)nesc;
            }
            szMatchBuf[idst] = '\0';
            szMatchEsc[idst] = '\0';
            ppat += isrc;
            if (cs.debug)
               printf("match:   check part \"%s\" cnext \"%c\" with hay at \"%.10s\"\n",szMatchBuf,cnext,phay);
            // find next occurrence of non-* part
            bool bsubmatch=0;
            while (*phay) {
               while (*phay && !cmpchr(*phay,cnext,bCase,enext)) phay++;
               if (!*phay) break;
               if (cs.debug)
                  printf("match:   from %.10s\n",phay);
               // matched first char, compare rest
               int isub=0;
               char *ppat2=szMatchBuf;
               bool *epat2=szMatchEsc;
               for (; phay[isub] && ppat2[isub]; isub++)
                  if (!cmpchr(phay[isub], ppat2[isub], bCase, epat2[isub]))
                     break;
               if (!ppat2[isub]) {
                  // rest matched: adapt hit positions
                  bsubmatch=1;
                  if (ifirst < 0)
                     ifirst = phay - pszHay;
                  ilast = phay - pszHay + isub - 1;
                  // jump past non-* part
                  if (cs.debug)
                     printf("match:   submatched \"%s\"\n",szMatchBuf);
                  phay += isub;
                  break;
               }
               // else retry from next position
               if (cs.debug)
                  printf("match:   submiss\n");
               phay++;
            }
            // synced past *
            if (!*ppat){
               if (bsubmatch) {
                  if (cs.debug)
                     printf("match:   full inner match, %d %d\n",ifirst,ilast);
                  bmatch=1;
               } else {
                  if (cs.debug)
                     printf("match:   full inner miss\n");
               }
               break; // match or miss
            }
            if (!*phay) break; // miss
            if (cs.debug)
               printf("match:   cont hay \"%.10s\" pat \"%.10s\"\n",phay,ppat);
            continue;
         }
         else
         {
            // compare single char, adapt match positions
            if (!cmpchr(*phay, cpat, bCase, 0)) {
               if (cs.debug)
                  printf("match:   miss at haychr \"%c\" msk %c position %d\n",*phay,cpat,phay-pszHay);
               break; // miss
            }
            if (ifirst < 0)
               ifirst = phay - pszHay;
            ilast  = phay - pszHay;
            phay++;
            // and continue
         }
      }  // end inner search loop
 
      if (cs.debug)
         printf("match:  1) ifirst %d ilast %d bmatch %d\n",ifirst,ilast,bmatch);

      if (bmatch) {
         // full hit somewhere, do we accept?
         if (!bTail) break;
         if (nhaylen > 0 && ilast == nhaylen-1) break;
         if (cs.debug)
            printf("match:  c) no line-end hit, %d != %d\n",ilast,nhaylen-1);
         // tail, and no hit at end: continue searching
      }
 
      if (ifirst < 0) break; // full miss
 
      ibase = ifirst+1; // try again
   }
   while (ibase < nhaylen);
 
   if (cs.debug)
      printf("match:  2) ifirst %d ilast %d bmatch %d for hay \"%s\" pat \"%s\"\n",ifirst,ilast,bmatch,pszHay,pszPat);

   if (bmatch && ifirst >= 0) {
      rfirsthit  = ifirst;
      if (ilast >= ifirst)
         rhitlen = (ilast-ifirst)+1;
      return 1;
   }

   return 0;
}

bool equalFileName(char *psz1, char *psz2) {
   #ifdef _WIN32
   // Windows: expect case-insensitive filenames
   return (!_stricmp(psz1, psz2)) ? 1 : 0;
   #else
   // Unix: expect case-sensitive filenames
   return (!strcmp(psz1, psz2)) ? 1 : 0;
   #endif
}

bool strBegins(char *pszStr, cchar *pszPat) {
   if (!strncmp(pszStr, pszPat, strlen(pszPat)))
      return 1;
   return 0;
}

bool strbeg(char *pszStr, cchar *pszPat) {
   if (!strncmp(pszStr, pszPat, strlen(pszPat)))
      return 1;
   return 0;
}

bool striBegins(char *pszStr, cchar *pszPat) {
   if (!mystrnicmp(pszStr, pszPat, strlen(pszPat)))
      return 1;
   return 0;
}

bool strcase(char *pszStr, cchar *pszPat) {
   return strcmp(pszStr, pszPat) ? 0 : 1;
}

bool strEnds(char *pszStr, cchar *pszPat) {
   int nlenhay = strlen(pszStr);
   int nlenpat = strlen(pszPat);
   if (nlenhay < nlenpat) return 0;
   return !strcmp(pszStr+nlenhay-nlenpat, pszPat) ? 1 : 0;
}

bool striEnds(char *pszStr, cchar *pszPat) {
   int nlenhay = strlen(pszStr);
   int nlenpat = strlen(pszPat);
   if (nlenhay < nlenpat) return 0;
   return !mystricmp(pszStr+nlenhay-nlenpat, pszPat) ? 1 : 0;
}

bool strEndsWith(char *psz, char c)
{
   int ilen=strlen(psz);
   if (ilen>0 && psz[ilen-1]==c)
      return 1;
   return 0;
}

char *dataAsHex(void *pAnyData, int iDataSize, char *pszBuf, int iMaxBuf, bool bLowerCase)
{
   static char szBuf[300];

   if (!pszBuf)
   {
      pszBuf = szBuf;
      iMaxBuf = sizeof(szBuf);
   }
 
   uchar *pSrcCur = (uchar *)pAnyData;
   uchar *pSrcMax = pSrcCur + iDataSize;
 
   char *pszDstCur = pszBuf;
   char *pszDstMax = pszBuf + iMaxBuf - 20;
 
   while (pSrcCur < pSrcMax && pszDstCur < pszDstMax)
   {
      uchar uc = *pSrcCur++;
      if (bLowerCase)
         sprintf(pszDstCur, "%02x", uc);
      else
         sprintf(pszDstCur, "%02X", uc);
      pszDstCur += 2;
   }
 
   *pszDstCur = '\0';
 
   return pszBuf;
}

char *dataAsTrace(void *pAnyData, int iDataSize, char *pszBuf, int iMaxBuf)
{
   static char szBuf[300];

   if (iDataSize == -1)
       iDataSize = strlen((char*)pAnyData);

   if (!pszBuf)
   {
      pszBuf = szBuf;
      iMaxBuf = sizeof(szBuf);
   }
 
   uchar *pSrcCur = (uchar *)pAnyData;
   uchar *pSrcMax = pSrcCur + iDataSize;
 
   char *pszDstCur = pszBuf;
   char *pszDstMax = pszBuf + iMaxBuf - 20;
 
   while (pSrcCur < pSrcMax && pszDstCur < pszDstMax)
   {
      uchar uc = *pSrcCur++;
 
      if (isprint((char)uc))
      {
         *pszDstCur++ = (char)uc;
         continue;
      }

      // convert binary to {hex}
      sprintf(pszDstCur, "{%02X}", uc);
      pszDstCur += 4;
   }
 
   *pszDstCur = '\0';
 
   return pszBuf;
}

char *dataAsTraceW(ushort *pAnyData)
{
   static char szBuf[300];

   char *pszBuf = szBuf;
   int  iMaxBuf = sizeof(szBuf);
 
   ushort *pSrcCur = (ushort *)pAnyData;
 
   char *pszDstCur = pszBuf;
   char *pszDstMax = pszBuf + iMaxBuf - 20;
 
   int i=0;
   for (; pSrcCur[i] != 0 && pszDstCur < pszDstMax; i++)
   {
      ushort uc = pSrcCur[i];
 
      if (uc < 0x100U && isprint((char)uc))
      {
         *pszDstCur++ = (char)uc;
         continue;
      }

      // convert binary to {hex}
      sprintf(pszDstCur, "{%04X}", uc);
      pszDstCur += 6;
   }
 
   *pszDstCur = '\0';
 
   return pszBuf;
}

uchar twoHexToUChar(char *psz, bool &rError)
{
   uchar u = 0;
   switch (psz[0])
   {
      case '0': u=0x00; break; case '1': u=0x10; break; case '2': u=0x20; break;
      case '3': u=0x30; break; case '4': u=0x40; break; case '5': u=0x50; break;
      case '6': u=0x60; break; case '7': u=0x70; break; case '8': u=0x80; break;
      case '9': u=0x90; break; case 'A': u=0xA0; break; case 'B': u=0xB0; break;
      case 'C': u=0xC0; break; case 'D': u=0xD0; break; case 'E': u=0xE0; break;
      case 'F': u=0xF0; break;
      default:
         rError=1;
         return 0;
   }
   switch (psz[1])
   {
      case '0': u|=0x00; break; case '1': u|=0x01; break; case '2': u|=0x02; break;
      case '3': u|=0x03; break; case '4': u|=0x04; break; case '5': u|=0x05; break;
      case '6': u|=0x06; break; case '7': u|=0x07; break; case '8': u|=0x08; break;
      case '9': u|=0x09; break; case 'A': u|=0x0A; break; case 'B': u|=0x0B; break;
      case 'C': u|=0x0C; break; case 'D': u|=0x0D; break; case 'E': u|=0x0E; break;
      case 'F': u|=0x0F; break;
      default:
         rError=1;
         return 0;
   }
   return u;
}

uint currentProcessID()
{
   #ifdef _WIN32
   return (uint)GetCurrentProcessId();
   #else
   return (uint)getpid();
   #endif
}

char *skipDotSlash(char *psz) {
   if (!strncmp(psz, glblDotSlash, 2))
      psz += 2;
   return psz;
}

bool endsWithPathChar(char *pszPath, bool bAcceptFWSlash=0) {
   int nLen = strlen(pszPath);
   if (nLen>0) {
      if (pszPath[nLen-1] == glblPathChar)
         return 1;
      if (bAcceptFWSlash && pszPath[nLen-1] == '/')
         return 1;
   }
   return 0;
}

bool endsWithColon(char *pszPath) {
   #ifdef _WIN32
   int nLen = strlen(pszPath);
   if (nLen>0 && pszPath[nLen-1] == ':')
      return 1;
   #endif
   return 0;
}

#ifdef SFK_LINUX_FULL
// experimental: change thread priority
void setPriority(int nprio)
{
   // supported values are -2 to +2
   #ifdef _WIN32
   SetThreadPriority(GetCurrentThread(), nprio);
   #else
   sched_param oschedparm;
   int nNativePrio = 0;
   switch (nprio) {
      case -2 : nNativePrio =   1; break;
      case -1 : nNativePrio =  25; break;
      case  0 : nNativePrio =  50; break;
      case  1 : nNativePrio =  75; break;
      case  2 : nNativePrio = 100; break;
   }
   if (nprio < -2) nNativePrio =   1;
   if (nprio >  2) nNativePrio = 100;
   oschedparm.sched_priority = nNativePrio;
   pthread_setschedparam(pthread_self(), SCHED_OTHER, &oschedparm);
   #endif
}
#endif

int containsWildCards(char *pszName)
{
   if (strchr(pszName, glblWildChar)) return 1;
   if (strchr(pszName, '*')) return 1;
   if (strchr(pszName, '?')) return 1;
   return 0;
}

int lastCharIsBackSlash(char *pszName)
{
   uint nlen = strlen(pszName);
   if (!nlen) return 0;
   return (pszName[nlen-1] == glblPathChar) ? 1 : 0;
}

void stripTrailingBackSlashes(char *psz)
{
   int nidx = (int)strlen(psz);
   while (nidx > 0 && psz[nidx-1] == glblPathChar) {
      psz[nidx-1] = '\0';
      nidx--;
   }
}

bool isHttpURL(char *psz)
{
   if (strBegins(psz, "http://"))
      return 1;
   if (strBegins(psz, "https://"))
      return 1;
   return 0;
}

void trimCR(char *pszBuf) {
   int ilen = strlen(pszBuf);
   if (ilen > 0 && pszBuf[ilen-1] == '\r')
      pszBuf[ilen-1] = '\0';
}

void removeCRLF(char *pszBuf) {
   char *pszLF = strchr(pszBuf, '\n');
   if (pszLF) *pszLF = '\0';
   char *pszCR = strchr(pszBuf, '\r');
   if (pszCR) *pszCR = '\0';
}

void fixPathChars(char *pszBuf) {
   // if path contains foreign path chars, change to local
   char *psz = pszBuf;
   for (; *psz; psz++)
      if (*psz == glblWrongPChar)
          *psz = glblPathChar;
}

int myfseek(FILE *f, num nOffset, int nOrigin)
{
   if (nOrigin != SEEK_SET)
      return 9+perr("internal: myfseek: supports only SEEK_SET");

   #ifdef _WIN32
   if (sizeof(num) != sizeof(fpos_t)) return 9+perr("internal: myfseek: wrong fpos_t size, need 64 bits");
   return fsetpos(f, &nOffset);
   #else
   fpos64_t xpos;
   if (fgetpos64(f, &xpos)) // read xpos.__state
      return 9+perr("internal: myfseek: failed to read position");
    #if defined(MAC_OS_X) || defined(SOLARIS)
     xpos = (fpos_t)nOffset;
    #else
     if (sizeof(xpos.__pos) != sizeof(nOffset)) return 9+perr("internal: myfseek: wrong __pos size, need 64 bits");
     xpos.__pos = nOffset;
    #endif
   return fsetpos64(f, &xpos);
   #endif
}

uchar aGlblGetBuf[MY_GETBUF_MAX+100];
int   nGlblGetSize  = 0;
int   nGlblGetIndex = 0;
int   nGlblGetEOD   = 0;
num   nGlblGetFPos  = 0;
#ifdef _WIN32
HANDLE hGlblMyFGetsStdIn = INVALID_HANDLE_VALUE;
#else
FILE *fGlblMyFGetsStdIn = 0;
#endif

/*
   replacement for (f)gets
   -  which cannot cope with 0x00 (and 0x1A under windows)
   -  which drops line ends randomly under VC14 Win32 WinMain
*/

void myfgets_init(bool bReadStdIn)
{
   nGlblGetSize  = 0;
   nGlblGetIndex = 0;
   nGlblGetEOD   = 0;
   nGlblGetFPos  = 0;

   #ifdef _WIN32
   if (bReadStdIn)
      hGlblMyFGetsStdIn = GetStdHandle(STD_INPUT_HANDLE);
   #else
   if (bReadStdIn)
      fGlblMyFGetsStdIn = stdin;
   #endif
}

int myfgets(char *pszOutBuf, int nOutBufLen, FILE *fin, bool *rpIsBinary, char *pAttrBuf)
{
   if (nGlblGetSize  < 0 || nGlblGetSize  > MY_GETBUF_MAX) return 0+perr("int. #62 %d %d\n",(nGlblGetSize < 0),(nGlblGetSize > MY_GETBUF_MAX));
   if (nGlblGetIndex < 0 || nGlblGetIndex > MY_GETBUF_MAX) return 0+perr("int. #63 %d %d\n",(nGlblGetIndex < 0),(nGlblGetIndex > MY_GETBUF_MAX));
   if (nGlblGetIndex > nGlblGetSize) return 0+perr("int. #64\n");
   if (nGlblGetEOD > 1) return 0+perr("int. #65\n");

   int nBufFree = MY_GETBUF_MAX - nGlblGetSize;
   uchar *pRead  = &aGlblGetBuf[nGlblGetSize];

   // refill read buffer
   int nRead = 0;
   if (!nGlblGetEOD && (nGlblGetSize < MY_GETBUF_MAX/2))
   do
   {
      #ifdef _WIN32
      if (fin == 0) {
         if (hGlblMyFGetsStdIn == INVALID_HANDLE_VALUE)
            return 0+perr("myfgets missing handle\n");
         DWORD nRead2 = 0;
         if (!ReadFile(hGlblMyFGetsStdIn, pRead, nBufFree, &nRead2, 0))
            nGlblGetEOD = 1;
         else
            nGlblGetSize += nRead2;
         nRead = nRead2;
         break;
      }
      #else
      if (fin == 0) {
         if (fGlblMyFGetsStdIn == 0) // i.e. no pointer set
            return 0+perr("myfgets missing handle\n");
         fin = fGlblMyFGetsStdIn;
      }
      #endif

      if ((nRead = fread(pRead, 1, nBufFree, fin)) <= 0)
         nGlblGetEOD = 1;
      else
         nGlblGetSize += nRead;
   }
   while (0);

   if (cs.debug) printf("] pre size %d index %d free %d nread %d\n", nGlblGetSize, nGlblGetIndex, nBufFree, nRead);

   // anything remaining?
   if (nGlblGetIndex >= nGlblGetSize) {
      nGlblGetEOD = 2;
      return 0;
   }

   // copy next line from front
   int nIndex     = nGlblGetIndex;
   int nOutIndex  = 0;
   int nOutSecLen = nOutBufLen-10;
   bool bBinary    = 0;
   for (; (nIndex < nGlblGetSize) && (nOutIndex < nOutSecLen);)
   {
      uchar c1 = aGlblGetBuf[nIndex++];
      nGlblGetFPos++; // count source position in file

      if (c1 == 0x00 || c1 == 0x1A) {
         if (!c1)
            bBinary = 1;
         c1 = (uchar)'.';
      }
      else
      if (c1 == (uchar)'\r')
         continue;

      pszOutBuf[nOutIndex++] = (char)c1;

      if (c1 == (uchar)'\n')
         break;
   }
   pszOutBuf[nOutIndex] = '\0';

   // promote binary flag
   if (bBinary && rpIsBinary)
      *rpIsBinary = 1;

   // move remaining cache data
   int nCacheRemain = nGlblGetSize-nIndex;
   if (nIndex + nCacheRemain < 0) return 0+perr("int. #60\n");
   if (nIndex + nCacheRemain > MY_GETBUF_MAX) return 0+perr("int. #61\n");
   if (nCacheRemain > 0)
      memmove(aGlblGetBuf, &aGlblGetBuf[nIndex], nCacheRemain);

   nGlblGetSize -= nIndex;
   nGlblGetIndex = 0;

   // if (cs.debug) printf("] pos size %d index %d out %d\n", nGlblGetSize, nGlblGetIndex, nOutIndex);

   return nOutIndex;
}

int isDirByName(char *pszName)
{
   // used for not-yet-existing targets:
   // tell if it should be a dir by looking at the name
   int nlen = strlen(pszName);
   if (nlen <= 0) return 0;
   if (!strcmp(pszName, ".")) return 1;
   if (!strcmp(pszName, "..")) return 1;
   char clast = pszName[nlen-1];
   if (clast == glblPathChar) return 1;
   #ifdef _WIN32
   if (clast == ':') return 1;
   #endif
   // todo: aaa/. and aaa/.. detection
   return 0;
}

int isDir(char *pszName)
{
   if (bGlblSyntaxTest) {
      // if just simulating, check how the name looks
      if (!strcmp(pszName, ".")) return 1;
      return strstr(pszName, "dir") ? 1 : 0;
   }
   if (containsWildCards(pszName))
      return 0;

   #ifdef _WIN32

   DWORD nAttrib = GetFileAttributes(pszName);
   if (nAttrib == 0xFFFFFFFF) // "INVALID_FILE_ATTRIBUTES"
      return 0;
   if (nAttrib & FILE_ATTRIBUTE_DIRECTORY)
      return 1;

   #else

   struct stat64 buf;
   if (stat64(pszName, &buf))
      return 0;
   if (buf.st_mode & _S_IFDIR )
      return 1;

   #endif

   return 0;
}

int fileExists(char *pszName, bool bOrDir)
{
   #ifdef _WIN32

   DWORD nAttrib = GetFileAttributes(pszName);
   if (nAttrib == 0xFFFFFFFF) // "INVALID_FILE_ATTRIBUTES"
      return 0;
   if (!bOrDir && (nAttrib & FILE_ATTRIBUTE_DIRECTORY))
      return 0; // is a dir, not a file

   #else

   struct stat64 buf;
   if (stat64(pszName, &buf))
      return 0;
   if (!bOrDir && (buf.st_mode & _S_IFDIR))
      return 0; // is a dir, not a file

   #endif

   // we can get the attribs, and it's not a dir,
   // so expect that it is a file.
   // TODO: maybe further checks should be added.
   return 1;
}

int coiExists(char *pszName, bool bOrDir)
{
   Coi *pcoi = new Coi(pszName, 0);
   if (!pcoi) return 0;
   pcoi->incref("cex");

   int nrc = pcoi->existsFile(bOrDir);

   if (!pcoi->decref())
      delete pcoi;

   return nrc;
}

#ifdef SFK_W64
int mystat64(char *pszName, struct __stat64 *pout)
{
   #if (_MSC_VER >= 1900)

   memset(pout, 0, sizeof(*pout));

   /*
      sometimes win10 produces a junk file creation date before 1970.
      then the visual 2015 runtime refuses to stat on that,
      with errno 132 "Overflow".
      mystat64 returns zero time in such a case.
   */

   WIN32_FILE_ATTRIBUTE_DATA oinf;
   int bok = GetFileAttributesEx(pszName, GetFileExInfoStandard, &oinf);
   if (!bok) return 9;

   pout->st_atime = fileTimeToTimeT(&oinf.ftLastAccessTime);
   pout->st_ctime = fileTimeToTimeT(&oinf.ftCreationTime);
   pout->st_mtime = fileTimeToTimeT(&oinf.ftLastWriteTime);
   unum nsize =      (((unum)oinf.nFileSizeHigh) << 32)
                  |  (((unum)oinf.nFileSizeLow)  <<  0);
   pout->st_size  = nsize;

   uint nmode = _S_IREAD | _S_IWRITE;

   if (oinf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      nmode |= _S_IFDIR;

   if (oinf.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      nmode &= (0xFFFFFFFFU ^ _S_IWRITE);

   pout->st_mode = nmode;

   return 0;

   #else

   return _stat64(pszName, pout);

   #endif
}
#endif

int getFileStat( // RC == 0 if exists anything
   char  *pszName,
   int   &rbIsDirectory,
   int   &rbCanRead,
   int   &rbCanWrite,
   num   &rlFileTime,
   num   &rlFileSize,
   num   *ppcatimes,     // optional: creation and access time
   void  *prawstat,      // optional: create copy of stat structure
   int    nrawstatmax,   // size of above buffer
   uint   nmodeflags     // bit 0: use alternative stat, if available
 )
{
   bool bAltStat = (nmodeflags & 1) ? 1 : 0;

   if (prawstat) memset(prawstat, 0, nrawstatmax);

   #ifdef _WIN32

   #ifdef WINFULL
   // special case: top level of UNC paths
   // \\host\root          fails with stat()
   // \\host\root\subdir   can be used
   if (    bAltStat
       || !strncmp(pszName, "\\\\", 2)
       || !strncmp(pszName, "//", 2)
      )
   do
   {
      if (!bAltStat)
      {
         // check UNC path format
         char  psep    = pszName[0];
         char *pszHost = pszName+2;
         char *pszRoot = strchr(pszHost, psep);
         if (!pszRoot) return -1; // wrong format
         pszRoot++;
         char *pszSub  = strchr(pszRoot, psep);
         if (pszSub)
            break; // not top level, fall through
      }
      mtklog(("GetFileAttributesEx %s", pszName));
      WIN32_FILE_ATTRIBUTE_DATA oinf;
      if (!GetFileAttributesEx(pszName, GetFileExInfoStandard, &oinf))
         return -1;
      uint nattrib = oinf.dwFileAttributes;
      rbIsDirectory = (nattrib & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
      rbCanRead     = 1;
      rbCanWrite    = (nattrib & FILE_ATTRIBUTE_READONLY)  ? 0 : 1;
      // on old msvc, this may be 0xFFFF... for timestamps > 2038:
      rlFileSize    =  (((num)oinf.nFileSizeHigh) << 32)
                     | ((num)oinf.nFileSizeLow);
      rlFileTime    =  fileTimeToTimeT(&oinf.ftLastWriteTime);
      if (ppcatimes != 0) {
         ppcatimes[0] = fileTimeToTimeT(&oinf.ftCreationTime);
         ppcatimes[1] = fileTimeToTimeT(&oinf.ftLastAccessTime);
      }
      return 0;
   }
   while (0);
   #else
   uint nattrib = GetFileAttributesA(pszName);
   rbIsDirectory = (nattrib & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
   rbCanRead     = 1;
   rbCanWrite    = (nattrib & FILE_ATTRIBUTE_READONLY)  ? 0 : 1;
   #endif

   // using MSC specific 64-bit filesize and time stamp infos
   #ifdef SFK_W64
   struct __stat64 buf;
   if (mystat64(pszName, &buf))
      return -1;
   #else
   struct stat buf;
   if (stat(pszName, &buf))
      return -1;
   #endif

   rbIsDirectory = (buf.st_mode & _S_IFDIR ) ? 1 : 0;
   rbCanRead     = (buf.st_mode & _S_IREAD ) ? 1 : 0;
   rbCanWrite    = (buf.st_mode & _S_IWRITE) ? 1 : 0;
   // on old msvc, this may be 0xFFFF... for timestamps > 2038.
   // diff msvc14 vs. msvc7:
   rlFileTime    =  buf.st_mtime;
   rlFileSize    =  buf.st_size;
   if (ppcatimes != 0) {
      ppcatimes[0] = buf.st_ctime;
      ppcatimes[1] = buf.st_atime;
   }
   if (prawstat) {
      if (nrawstatmax < sizeof(buf))
         return 9+perr("internal #1090: statbuf too small\n");
      memcpy(prawstat, &buf, sizeof(buf));
   }
   return 0;

   #else

   // generic linux 64-bit stat
   struct stat64 buf;
   if (stat64(pszName, &buf))
      return -1;
   rbIsDirectory = (buf.st_mode & _S_IFDIR ) ? 1 : 0;
   rlFileTime    =  buf.st_mtime;
   rlFileSize    =  buf.st_size;
   rbCanRead     = (buf.st_mode & _S_IREAD ) ? 1 : 0;
   rbCanWrite    = (buf.st_mode & _S_IWRITE) ? 1 : 0;
   if (ppcatimes != 0) {
      ppcatimes[0] = buf.st_ctime;
      ppcatimes[1] = buf.st_atime;
   }
   if (prawstat) {
      if (nrawstatmax < (int)sizeof(buf))
         return 9+perr("internal #1090: statbuf too small\n");
      memcpy(prawstat, &buf, sizeof(buf));
   }
   return 0;

   #endif
}

num getFileSize(char *pszName)
{
   int bIsDir    = 0;
   int bCanRead  = 1;
   int bCanWrite = 1;
   num  nFileTime = 0;
   num  nFileSize = 0;
   if (getFileStat(pszName, bIsDir, bCanRead, bCanWrite, nFileTime, nFileSize))
      return -1;
   return nFileSize;
}

num getFileSizeSeek(char *pszName)
{
   FILE *fin = fopen(pszName, "rb");
   if (!fin) return -1;

   #ifdef SFK_W64
   if (_fseeki64(fin, 0, SEEK_END))
   #else
   if (fseek(fin, 0, SEEK_END))
   #endif
   {
      fclose(fin);
      return -1;
   }

   #ifdef SFK_W64
   num npos = (num)_ftelli64(fin);
   #else
   num npos = (num)ftell(fin);
   #endif

   fclose(fin);

   return npos;
}

num getFileTime(char *pszName)
{
   int bIsDir    = 0;
   int bCanRead  = 1;
   int bCanWrite = 1;
   num  nFileTime = 0;
   num  nFileSize = 0;
   if (getFileStat(pszName, bIsDir, bCanRead, bCanWrite, nFileTime, nFileSize))
      return 0;
   return nFileTime;
}

int mySetFileTime(char *pszFile, num nTime)
{
   FileStat ofs;
   if (ofs.readFrom(pszFile)) {
      pinf("cannot read file time: %s\n", pszFile);
      if (nTime)
         return 9;
   } else {
      if (nTime) {
         ofs.src.nMTime = nTime;
         ofs.src.nCTime = nTime;
         #ifdef _WIN32
         ofs.src.nHaveWFT = 0;
         #endif
      }
      ofs.writeTo(pszFile, __LINE__, 1); // 1: write just time
   }

   return 0;
}

// get file age in seconds
num getFileAge(char *pszName)
{
   num nFileTime = getFileTime(pszName);
   // printf("filetime: %s\n",numtoa(nFileTime));
   num nNow = getSystemTime();
   // printf("now is  : %s\n",numtoa(nNow));
   num nAge = nNow - nFileTime;
   return nAge;
}

bool canWriteFile(char *pszName, bool bTryCreate)
{
   int bIsDir    = 0;
   int bCanRead  = 0;
   int bCanWrite = 0;
   num  nFileTime = 0;
   num  nFileSize = 0;
   if (!getFileStat(pszName, bIsDir, bCanRead, bCanWrite, nFileTime, nFileSize)) {
      return bCanWrite ? 1 : 0;
   }
   if (bTryCreate) {
      // file does not exist yet: try creation
      FILE *fout = fopen(pszName, "wb");
      if (!fout) return 0;
      fclose(fout);
      remove(pszName);
      return 1;
   }
   return 1;
}

// uses szLineBuf.
int getFileSystemInfoRaw(
   char  *pszPath,          // e.g. "D:\\", "/home/user/"
   num   &nOutTotalBytes,   // total volume size
   num   &nOutFreeBytes,    // free bytes usable for normal users
   char  *pszOutFSName,     // file system name buffer
   int    nOutFSNMaxSize,   // size of this buffer
   char  *pszOutVolID,      // volume name and serial, if any
   int    nOutVolIDMaxSize, // size of this buffer
   uint  &rOutVolID         // numeric volume id
   )
{
   nOutTotalBytes  = -1;
   nOutFreeBytes   = -1;
   pszOutFSName[0] = '\0';
   pszOutVolID[0]  = '\0';
   rOutVolID       =  0;

   #ifdef _WIN32

   #ifdef WINFULL
   char  szVolName[200];
   DWORD nVolSerNum = 0;
   DWORD nMaxFNLen  = 0;
   DWORD nFSFlags   = 0;

   if (!GetVolumeInformation(
      pszPath,
      szVolName, sizeof(szVolName)-10,
      &nVolSerNum, &nMaxFNLen, &nFSFlags,
      pszOutFSName, nOutFSNMaxSize
      ))
      return 9+perr("unable to get volume information for %s\n", pszPath);

   szVolName[sizeof(szVolName)-10] = '\0';
   sprintf(szLineBuf, "%08lX %s", nVolSerNum, szVolName);
   mystrcopy(pszOutVolID, szLineBuf, nOutVolIDMaxSize);

   rOutVolID = nVolSerNum;

   ULARGE_INTEGER nFreeCaller;
   ULARGE_INTEGER nTotalBytes;
   ULARGE_INTEGER nFreeTotal;
   if (!GetDiskFreeSpaceEx(
      pszPath,
      &nFreeCaller, &nTotalBytes, &nFreeTotal
      ))
      return 9+perr("unable to get free space of %s\n", pszPath);

   nOutTotalBytes = nTotalBytes.QuadPart;
   nOutFreeBytes  = nFreeCaller.QuadPart;
 
   // sprintf(szLineBuf, "VOL: %X for %s",nVolSerNum,pszPath);
   // MessageBox(0, szLineBuf, "info", MB_OK);
   #endif

   return 0;

   #else

   // #include <sys/statvfs.h>
   struct statvfs64 oinf;
   if (statvfs64(pszPath, &oinf))
      return 9+perr("unable to get free space of %s\n", pszPath);

   // unsigned int f_bsize   - preferred filesystem blocksize.
   // unsigned int f_frsize  - fundamental filesystem blocksize (if supported)
   // fsblkcnt_t f_blocks     - total number of blocks on the filesystem, in units of f_frsize.
   // fsblkcnt_t f_bfree      - total number of free blocks.
   // fsblkcnt_t f_bavail     - number of free blocks available to a nonsuperuser.
   // fsfilcnt_t f_files      - total number of file nodes (inodes).
   // fsfilcnt_t f_ffree      - total number of free file nodes.
   // fsfilcnt_t f_favail     - number of inodes available to a nonsuperuser.
   // unsigned int f_fsid    - filesystem ID (dev for now).
   // char f_basetype[16]     - type of the target filesystem, as a null-terminated string.
   // unsigned int f_flag    - bitmask of flags; the function can set these flags:
   //    ST_RDONLY -- read-only filesystem.
   //    ST_NOSUID -- the filesystem doesn't support setuid/setgid semantics.
   // unsigned int f_namemax - maximum filename length.

   // not with linux:
   // mystrcopy(pszOutFSName, oinf.f_basetype, nOutFSNMaxSize);
   pszOutFSName[0] = '\0';

   num nTotalBytes = (num)oinf.f_blocks * (num)oinf.f_frsize;
   num nFreeBytes  = (num)oinf.f_bavail * (num)oinf.f_frsize;

   nOutTotalBytes = nTotalBytes;
   nOutFreeBytes  = nFreeBytes;

   return 0;

   #endif
}

int getFileSystemInfo(
   char  *pszPath,          // e.g. "D:\\", "/home/user/"
   num   &nOutTotalBytes,   // total volume size
   num   &nOutFreeBytes,    // free bytes usable for normal users
   char  *pszOutFSName,     // file system name buffer
   int    nOutFSNMaxSize,   // size of this buffer
   char  *pszOutVolID,      // volume name and serial, if any
   int    nOutVolIDMaxSize, // size of this buffer
   uint  &rOutVolID         // numeric volume id
   )
{
   char szPath[SFK_MAX_PATH+10];
   strcopy(szPath, (char*)pszPath);

   #ifdef _WIN32

   // need a root dir path like "C:"
   if (szPath[1] == ':') {
      // there is a C:, or D:, etc.
      szPath[2] = '\\';
      szPath[3] = '\0';
   } else {
      // a relative path: check for invalids
      if (!strncmp(szPath, "\\\\", 2))
         return -1;
      if (!strncmp(szPath, "//", 2))
         return -1;
      // find out our drive
      if (getcwd(szPath,SFK_MAX_PATH)) { }
      if (szPath[1] != ':')
         return -1;
      // we're on C:, or D:, etc.
      szPath[2] = '\\';
      szPath[3] = '\0';
   }

   #else

   // reduce /media/small/dummydir/whatever.txt
   // to the first existing directory
   while (1)
   {
      struct stat oinf;
      if (!stat(szPath, &oinf))
         break; // exists

      char *psz = strrchr(szPath, '/');
      if (!psz)
         return -1; // failed to reduce

      *psz = '\0';
      // and retry on next higher level
   }

   #endif

   return getFileSystemInfoRaw(szPath,
      nOutTotalBytes,
      nOutFreeBytes,
      pszOutFSName,
      nOutFSNMaxSize,
      pszOutVolID,
      nOutVolIDMaxSize,
      rOutVolID
      );
}

num getFreeSpace(char *pszPath)
{
   num nTotal=0, nFree=0;
   char szFSName[200];
   char szVolID[200];
   uint nVolID=0;
 
   if (getFileSystemInfo(pszPath, nTotal, nFree, szFSName, sizeof(szFSName)-10, szVolID, sizeof(szVolID)-10, nVolID))
      return -1;

   return nFree;
}

// result is always non NULL but can be empty string.
char *timeAsString(num nTime, int iMode, bool bUTC)
{
   static char szTimeStrBuf[200];

   // nTime may be 0xFFFF... in case of times > 2038.

   #ifdef SFK_VC14
   // msvc14 strftime crashes on negative times
   if (nTime < 0)
      return str("");
   #endif

   struct tm *pLocTime = 0;
   mytime_t nTime2 = (mytime_t)nTime;

   if (bUTC || cs.useutc) {
      pLocTime = mygmtime(&nTime2);    // may be NULL
   } else {
      pLocTime = mylocaltime(&nTime2); // may be NULL
   }

   bool bFlat    = (iMode & 1) ? 1 : 0;
   bool bFlatSep = (iMode & 2) ? 1 : 0;
   bool bTabSep  = (iMode & 4) ? 1 : 0;

   // size_t strftime( char *strDest, size_t maxsize, const char *format, const struct tm *timeptr );
   // 20110101 120101
   // 2011-01-01 12:01:01
   // 01234567890
 
   struct tm oNullTime;
   mclear(oNullTime);
 
   if (!pLocTime)
      pLocTime = &oNullTime;
 
   szTimeStrBuf[0] = '\0';

   if (bFlat) {
      if (bFlatSep) {
         strftime(szTimeStrBuf, sizeof(szTimeStrBuf)-10, "%Y%m%d %H%M%S", pLocTime);
         szTimeStrBuf[8] = bTabSep ? '\t' : ' ';
      } else {
         strftime(szTimeStrBuf, sizeof(szTimeStrBuf)-10, "%Y%m%d%H%M%S", pLocTime);
      }
   } else {
      strftime(szTimeStrBuf, sizeof(szTimeStrBuf)-10, "%Y-%m-%d %H:%M:%S", pLocTime);
      szTimeStrBuf[10] = bTabSep ? '\t' : ' ';
   }

   return szTimeStrBuf;
}

static const char *pszGlblMonths[] = {
   "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

bool tryGetRelTime(cchar *psz, num &nRetTime)
{
   bool brc = false;

   // get local time (zone)
   mytime_t now = mytime(NULL);
   struct tm *tm = 0;
   tm = mylocaltime(&now);
   tm->tm_isdst = -1;

   int nlen = strlen(psz);

   if (!strcmp(psz, "now")) {
      brc = true;
   }
   else
   if (!strcmp(psz, "today")) {
      tm->tm_hour = 0;
      tm->tm_min  = 0;
      tm->tm_sec  = 0;
      brc = true;
   }
   else
   if (!strcmp(psz, "tomonth")) {
      tm->tm_hour = 0;
      tm->tm_min  = 0;
      tm->tm_sec  = 0;
      tm->tm_wday = 0;  // 0..6
      tm->tm_mday = 1;  // 1..31
      brc = true;
   }
   else
   if (!strcmp(psz, "toyear")) {
      tm->tm_hour = 0;
      tm->tm_min  = 0;
      tm->tm_sec  = 0;
      tm->tm_wday = 0;  // 0..6
      tm->tm_mday = 1;  // 1..31
      tm->tm_yday = 0;  // 0..365
      tm->tm_mon  = 0;  // 0..11
      brc = true;
   }
   else
   if (nlen >= 2 && nlen <= 4 && psz[nlen-1] == 'd') {
      mytime_t pre = now - atol(psz) * 3600 * 24;
      tm = mylocaltime(&pre);
      tm->tm_isdst = -1;
      brc = true;
   }
   else
   if (nlen >= 2 && nlen <= 4 && psz[nlen-1] == 'h') {
      mytime_t pre = now - atol(psz) * 3600;
      tm = mylocaltime(&pre);
      tm->tm_isdst = -1;
      brc = true;
   }
   else
   if (nlen >= 2 && nlen <= 4 && psz[nlen-1] == 'm') {
      mytime_t pre = now - atol(psz) * 60;
      tm = mylocaltime(&pre);
      tm->tm_isdst = -1;
      brc = true;
   }
   else
   if (nlen >= 2 && nlen <= 4 && psz[nlen-1] == 's') {
      mytime_t pre = now - atol(psz);
      tm = mylocaltime(&pre);
      tm->tm_isdst = -1;
      brc = true;
   }

   mytime_t nTime = mymktime(tm);
   if (!nTime) { perr("cannot calc time: %s\n", psz); return 0; }
   num nTime2 = (num)nTime;
   if (nTime2 <= 0) { perr("cannot calc time: %s\n", psz); return 0; }

   nRetTime = nTime2;
   return brc;
}

num localTimeZoneOffset(mytime_t t1)
{
   struct tm *ptm = mygmtime(&t1);

   // we want pure tzo without dst
   ptm->tm_isdst = 0;

   // mktime expects LOCAL time.
   mytime_t t2 = mymktime(ptm);
   // so now t2 differs from t1.

   num t3 = (num)difftime(t1, t2);

   return t3;
}

int timeFromString(char *pszin, num &nRetTime, bool bsilent, bool bUTC)
{
   char *psz = pszin;

   // get local time
   mytime_t now = mytime(NULL);

   struct tm *tm = 0;
   tm = mylocaltime(&now);

   // mktime below must figure out DST automatically
   tm->tm_isdst = -1;

   // accept formats:
   // 12345678901234567890123456789012345678901234567890
   // 01 Dec 2008 01:01:25 +0200 GMT - 30 chars, FAILS on wrong locale
   // 01 Dec 2008 01:01:25 +0100 - 26 chars, FAILS on wrong locale
   // 01 Dec 2008 01:01:25 GMT   - 24 chars, FAILS on wrong locale
   // 2006-11-21 12:49:36  - 19 chars
   // 2006-11-21           - 10 chars
   // 20061121124936       - 14 chars
   // 20061121             - 08 chars
   // Sep 28 2006          - 11 chars
   // Sep 28 14:37         - 12 chars
   uint nyear=0,nmon=0,nday=0,nhour=0,nmin=0,nsec=0;
   int nslen = strlen(psz);
   int lrc = 0;
   if (nslen == 24 || nslen == 26 || nslen == 30) {
      // TODO: so far, "GMT" or "+0100" postfixes are ignored.
      nday = atol(psz);
      psz += 3;
      char *pszMon = psz;
      for (nmon=0; nmon<12; nmon++)
         if (!strncmp(pszMon, pszGlblMonths[nmon], 3))
            break;
      if (nmon >= 12) {
         if (!bsilent && !cs.nonotes)
            pinf("unknown date/time locale: %s\n", psz);
         return 9;
      }
      psz += 4;
      lrc = sscanf(psz, "%4u %2u:%2u:%2u", &nyear, &nhour, &nmin, &nsec);
      if (lrc != 4) {
         if (!bsilent && !cs.nonotes)
            pinf("wrong date/time format.0: %s\n", psz);
         return 9;
      }
      nmon++; // ONE based months
   }
   else
   if (nslen == 19) {
      lrc = sscanf(psz, "%4u-%2u-%2u %2u:%2u:%2u", &nyear, &nmon, &nday, &nhour, &nmin, &nsec);
      if (lrc != 6) return 9+perr("wrong date/time format.1: %s\n", psz);
   }
   else
   if (nslen == 14) {
      lrc = sscanf(psz, "%4u%2u%2u%2u%2u%2u", &nyear, &nmon, &nday, &nhour, &nmin, &nsec);
      if (lrc != 6) return 9+perr("wrong date/time format.2: %s\n", psz);
   }
   else
   if (nslen == 12) {
      char *pszMon = psz;
      for (nmon=0; nmon<12; nmon++)
         if (!strncmp(pszMon, pszGlblMonths[nmon], 3))
            break;
      if (nmon >= 12) return 9+perr("wrong date/time format.3: %s\n", psz);
      psz += 4;
      lrc = sscanf(psz, "%2u %2u:%2u", &nday, &nhour, &nmin);
      if (lrc == 3) {
         // copy year from current time:
         nyear = tm->tm_year;
         if (nyear < 1970)
            nyear = nyear - 100 + 2000;   // "108" -> "2008"
      } else {
         // retry on "29  2007" with TWO blanks:
         lrc = sscanf(psz, "%2u  %4u", &nday, &nyear);
         if (lrc != 2) return 9+perr("wrong date/time format.4: %s\n", psz);
      }
      nmon++; // ONE based months
   }
   else
   if (nslen == 11) {
      char *pszMon = psz;
      for (nmon=0; nmon<12; nmon++)
         if (!strncmp(pszMon, pszGlblMonths[nmon], 3))
            break;
      if (nmon >= 12) return 9+perr("wrong date/time format.5: %s\n", psz);
      psz += 4;
      lrc = sscanf(psz, "%2u %4u", &nday, &nyear);
      if (lrc != 2) return 9+perr("wrong date/time format.6: %s\n", psz);
      nmon++; // ONE based months
   }
   else
   if (nslen == 10) {
      lrc = sscanf(psz, "%4u-%2u-%2u", &nyear, &nmon, &nday);
      if (lrc != 3) return 9+perr("wrong date format.7: %s\n", psz);
   }
   else
   if (nslen == 8) {
      lrc = sscanf(psz, "%4u%2u%2u", &nyear, &nmon, &nday);
      if (lrc != 3) return 9+perr("wrong date format.8: %s\n", psz);
   }
   else
      return 9+perr("wrong date and/or time format.9: \"%s\"\n", psz);

   if (nyear < 1970) return 9+perr("unexpected year: %u\n", nyear);
   if (nmon < 1 || nmon > 12) return 9+perr("unexpected month: %u in \"%s\" %d\n", nmon, pszin, nslen);
   if (nday < 1 || nday > 31) return 9+perr("unexpected day: %u\n", nday);
   if (nhour > 23) return 9+perr("unexpected hour: %u\n", nhour);
   if (nmin  > 59) return 9+perr("unexpected minute: %u\n", nmin);
   if (nsec  > 59) return 9+perr("unexpected second: %u\n", nsec);

   // adjust values
   nyear -= 1900;
   nmon--; // mktime uses ZERO BASED months

   // printf("] %u-%u-%u %u:%u:%u\n",nyear,nmon,nday,nhour,nmin,nsec);

   tm->tm_year = nyear;
   tm->tm_mon  = nmon;
   tm->tm_mday = nday;
   tm->tm_hour = nhour;
   tm->tm_min  = nmin;
   tm->tm_sec  = nsec;

   bool bNearEpoch = 0;
   if (nyear == 70 && nmon == 0 && nday == 1)
        bNearEpoch = 1;

   // mktime USES LOCAL TIME ONLY.
   // mkgmtime is no standard and available nowhere.
   mytime_t nTime = mymktime(tm);

   // if the string was UTC then nTime is now TOO SMALL
   // as the zone offset and DST was subtracted.
   // so if we need the UTC value we MUST do this:
   if (bUTC) nTime += localTimeZoneOffset(nTime);

   num nTime2 = (num)nTime;
   if (     nTime2 < 0
       || (!bNearEpoch && (nTime2 == 0))
      )
      return 9+perr("invalid date/time: %s (%d %d %d)\n", psz, nyear, nmon, nday);

   // char *psz1 = timeAsString(nTime2);
   // printf("] %s\n", psz1);

   nRetTime = nTime2;
   return 0;
}

char getYNAchar()
{
   while (1) {
      int c1 = getchar();
      if (c1=='\r' || c1=='\n')
         continue;
      return c1;
   }
}

// sizefromstr
num numFromSizeStr(char *psz, cchar *pszLoudInfo, bool bRelaxed)
{
   int nLen = strlen(psz);
   if (nLen >= 1) {
      num lNum = myatonum(psz);
      if (!strncmp(psz, "0x", 2))
         return lNum; // hex: always byte values
      char cPostFix = psz[nLen-1];
      switch (cPostFix) {
         case 'b': return lNum;
         case 'k': return lNum * 1000;
         case 'K': return lNum * 1024;
         case 'm': return lNum * 1000000;
         case 'M': return lNum * 1048576;
         case 'g': return lNum * 1000000000;
         case 'G': return lNum * 1073741824;
         default :
            if (bRelaxed)
               return lNum;
            if (cPostFix >= '0' && cPostFix <= '9')
               return lNum; // no postfix at all: assume bytes
      }
   }
   if (pszLoudInfo) {
      perr("unexpected %s value: %s", pszLoudInfo, psz);
      pinf("supply a value like 1000 500k 100m 5M [k=kbyte,m=mbyte]\n");
   }
   return -1;
}

num currentElapsedMSec() {
   return getCurrentTime() - nGlblStartTime;
}

uint currentKBPerSec() {
   num lMSElapsed = currentElapsedMSec();
   if (lMSElapsed == 0) lMSElapsed = 1;
   return (uint)(cs.totalbytes / lMSElapsed);
}

uint currentMBPerSec() {
   num lMSElapsed = currentElapsedMSec();
   if (lMSElapsed == 0) lMSElapsed = 1;
   return (uint)((cs.totalbytes / lMSElapsed) / 1000);
}

char szCmpBuf1[4096];
char szCmpBuf2[4096];

// 1694: new strstri without size limit
char *mystrstri(char *phay, cchar *ppat)
{
   int i=0,j=0,ncmp=0,ndiff=0;

   int llen = (int)strlen(phay);
   int slen = (int)strlen(ppat);

   ncmp = (llen-slen)+1;

   if (ncmp < 0)
      return 0;

   for (j=0; j<ncmp; ++j)
   {
      ndiff = 0;

      for (i=0; (i<slen) && !ndiff; ++i)
         ndiff = sfktolower(phay[j+i]) - sfktolower(ppat[i]);

      if (!ndiff)
         return (char *)phay+j;
   }

   return 0;
}

// 1694: replacement for old strstri
int mystrstrip(char *psz1, cchar *psz2, int *lpAtPosition)
{
   char *pHit = mystrstri(psz1, psz2);
   int iPos = pHit ? (int)(pHit - psz1) : -1;
   if (lpAtPosition)
      *lpAtPosition = iPos;
   return pHit ? 1 : 0;
}

// same as above, but support "?" wildcards.
// only for strings up to 4k length.
int mystrstriq4k(char *psz1, cchar *psz2, int *lpAtPosition=0)
{
   int slen1 = strlen(psz1);
   if (slen1 > (int)sizeof(szCmpBuf1)-10)
       slen1 = (int)sizeof(szCmpBuf1)-10;
   memcpy(szCmpBuf1, psz1, slen1);
   szCmpBuf1[slen1] = '\0';

   int slen2 = strlen(psz2);
   if (slen2 > (int)sizeof(szCmpBuf2)-10)
       slen2 = (int)sizeof(szCmpBuf2)-10;
   memcpy(szCmpBuf2, psz2, slen2);
   szCmpBuf2[slen2] = '\0';

   for (int i1=0; i1<slen1; i1++)
      szCmpBuf1[i1] = sfktolower(szCmpBuf1[i1]);
   for (int i2=0; i2<slen2; i2++) {
      szCmpBuf2[i2] = sfktolower(szCmpBuf2[i2]);
      if (glblUPatMode!=0 && szCmpBuf2[i2]==glblWrongPChar) // sfk183
         szCmpBuf2[i2] = glblPathChar;
   }

   char *pszHay = szCmpBuf1;  // HayStack
   char *pszNed = szCmpBuf2;  // Needle
   char *pszHit = 0;
   while (*pszHay) {
      // optim: quick check of current char
      if ((cs.noqwild == 1 || *pszNed != '?') // sfk1972 -noqwild
          && *pszHay != *pszNed)
         { pszHay++; continue; }
      // compare against needle at current hay position
      int i=0;
      for (; pszHay[i] && pszNed[i]; i++) {
         char c = pszNed[i];
         if (cs.noqwild == 0 && c == '?') // sfk1972 -noqwild
            continue;
         if (c != pszHay[i])
            break;
      }
      // comparison stopped: why?
      if (!pszNed[i]) {
         // needle elapsed: success
         pszHit = pszHay;
         break;
      }
      // needle not elapsed: no hit here
      pszHay++;
   }

   if (lpAtPosition) {
      if (pszHit)
         *lpAtPosition = (int)(pszHit - szCmpBuf1);
      else
         *lpAtPosition = -1;
   }

   return (pszHit != 0) ? 1 : 0;
}

// returns 0 if equal.
int mystrncmp(char *psz1, cchar *psz2, int nLen, bool bCase)
{
   if (bCase)
      return strncmp(psz1, psz2, nLen);

   int i=0;
   for (i=0; i<nLen && psz1[i] && psz2[i]; i++)
      if (sfktolower(psz1[i]) != sfktolower(psz2[i]))
         return 1;

   return (i==nLen) ? 0 : 1;
}

int mystricmp(char *psz1, cchar *psz2)
{
   while (*psz1 && *psz2 && sfktolower(*psz1) == sfktolower(*psz2)) {
      psz1++;
      psz2++;
   }
   return sfktolower(*psz1) - sfktolower(*psz2);
}

int mystrnicmp(char *psz1, cchar *psz2, int nLen)
{
   int i=0;
   for (i=0; i<nLen && psz1[i] && psz2[i]; i++)
      if (sfktolower(psz1[i]) != sfktolower(psz2[i]))
         return sfktolower(psz1[i]) - sfktolower(psz2[i]);

   if (i >= nLen) return 0;

   return sfktolower(psz1[i]) - sfktolower(psz2[i]);
}

bool stricase(char *psz, cchar *ppat)
   {  return mystricmp(psz, ppat) ? 0 : 1; }

bool strnicase(char *psz, cchar *pstart, int nlen)
   {  return mystrnicmp(psz, pstart, nlen) ? 0 : 1; }

bool stribeg(char *psz, cchar *pstart)
   {  return mystrnicmp(psz, pstart, strlen(pstart)) ? 0 : 1; }

struct SFKLoadNode
{
   struct SFKLoadNode *pNext;
   int iSize;
   char abData[1000000];
};

char *loadStdIn(num &rnFileSize)
{
   SFKLoadNode *pFirst=0,*pCur=0,*pPrev=0,*pDel=0;

   int iRC=0,iTotal=0;

   // load packages
   while (1)
   {
      if (!(pCur = new SFKLoadNode())) {
         perr("out of memory");
         iRC = 9;
         break;
      }
      memset(pCur, 0, sizeof(SFKLoadNode));

      int iRead = fread(pCur->abData, 1, sizeof(pCur->abData), stdin);

      if (iRead <= 0) {
         delete pCur;
         break;
      }

      if (!pFirst)
         pFirst = pCur;

      pCur->iSize = iRead;
      iTotal += iRead;

      if (pPrev)
         pPrev->pNext = pCur;

      pPrev = pCur;
   }

   // join packages
   char *pOut = 0;
 
   if (!iRC)
   {
      if (!(pOut = new char[iTotal+100]))
      {
         perr("out of memory");
         iRC = 10;
      }
      else
      {
         int iOff=0;
         for (pCur = pFirst; pCur; pCur=pCur->pNext)
         {
            memcpy(pOut+iOff, pCur->abData, pCur->iSize);
            iOff += pCur->iSize;
         }
         pOut[iOff] = '\0';
      }
   }
 
   // cleanup
   pCur = pFirst;
   while (pCur)
   {
      SFKLoadNode *pTmp = pCur->pNext;
      delete pCur;
      pCur = pTmp;
   }

   if (iRC)
      return 0;
 
   rnFileSize = iTotal;

   return pOut;
}

char *loadFile(char *pszFile, bool bquiet)
{
   num nFileSize = getFileSize(pszFile);
   if (nFileSize < 0) {
      if (!bquiet) perr("file not found: %s", pszFile);
      return 0;
   }

   num nTolerance = 10;
   char *pOut = new char[nFileSize+nTolerance+4];
   if (!pOut) {
      perr("out of memory: %s\n", pszFile);
      return 0;
   }
   memset(pOut+nFileSize, 0, nTolerance); // added safety

   FILE *fin = fopen(pszFile, "rb");
   if (!fin) {
      if (!bquiet) pferr(pszFile, "cannot read: %s\n", pszFile);
      delete [] pOut;
      return 0;
   }

   int nRead = fread(pOut, 1, nFileSize, fin);
   fclose(fin);
   if (nRead != nFileSize) {
      if (!bquiet) pferr(pszFile, "cannot read: %s (%d %d)\n", pszFile, nRead, nFileSize);
      delete [] pOut;
      return 0;
   }

   // loadFile guarantees zero termination after buffer:
   pOut[nFileSize] = '\0';
   return pOut;
}

uchar *loadBinaryFile(char *pszFile, num &rnFileSize)
{
   num nFileSize = getFileSize(pszFile);
   if (nFileSize < 0)
      return 0;

   num nTolerance = 10;
   char *pOut = new char[nFileSize+nTolerance+4];
   if (!pOut) {
      perr("out of memory: %s\n", pszFile);
      return 0;
   }
   memset(pOut+nFileSize, 0, nTolerance); // added safety

   FILE *fin = fopen(pszFile, "rb");
   if (!fin) {
      pferr(pszFile, "cannot read: %s\n", pszFile);
      delete [] pOut;
      return 0;
   }

   int nRead = fread(pOut, 1, nFileSize, fin);
   fclose(fin);
   if (nRead != nFileSize) {
      pferr(pszFile, "cannot read: %s (%d %d)\n", pszFile, nRead, nFileSize);
      delete [] pOut;
      return 0;
   }

   // not strictly needed w/binary data, but anyway.
   pOut[nFileSize] = '\0';

   rnFileSize = nFileSize;
   return (uchar*)pOut;
}

uchar *loadBinaryFlex(Coi &ocoi, num &rnFileSize)
{
   #ifdef VFILEBASE
   if (ocoi.preload("lbf", 0, 0)) // loadBinaryFlex
      return 0;
   #endif // VFILEBASE

   num nFileSize = ocoi.getSize();
   if (nFileSize < 0)
      return 0;

   num nTolerance = 10;
   char *pOut = new char[nFileSize+nTolerance+4];
   if (!pOut) {
      perr("out of memory: %s\n", ocoi.name());
      return 0;
   }
   memset(pOut+nFileSize, 0, nTolerance); // added safety

   if (ocoi.open("rb")) {
      pferr(ocoi.name(), "cannot read: %s\n", ocoi.name());
      delete [] pOut;
      return 0;
   }

   int nRead = ocoi.read(pOut, nFileSize);

   ocoi.close();

   if (nRead != nFileSize) {
      pferr(ocoi.name(), "cannot read: %s (%d %d)\n", ocoi.name(), nRead, nFileSize);
      delete [] pOut;
      return 0;
   }

   // not strictly needed w/binary data, but anyway.
   pOut[nFileSize] = '\0';

   rnFileSize = nFileSize;
   return (uchar*)pOut;
}

uchar *loadBinaryFlex(char *pszFile, num &rnFileSize)
{
   Coi *pcoi = new Coi(pszFile, 0);
   if (!pcoi) return 0;

   CoiAutoDelete odel(pcoi, 0); // no decref

   return loadBinaryFlex(*pcoi, rnFileSize);
}

int saveFile(char *pszName, uchar *pData, int iSize, const char *pszMode)
{
   FILE *fout = fopen(pszName, pszMode);
   if (!fout)
      return 9+perr("cannot write: %s\n", pszName);

   if (myfwrite(pData, iSize, fout) != iSize) {
      fclose(fout);
      return 10+perr("cannot fully write (disk full?): %s\n", pszName);
   }

   fclose(fout);
   return 0;
}

bool isEmptyDir(char *pszIn)
{
   bool bRC = 1;

   int lSize1       = strlen(pszIn);
   char *pszPattern  = new char[lSize1+10];
   char *pszBasePath = new char[lSize1+10];

   // pszIn might be
   // -  a directory, with or w/o slash at end
   // -  a pattern expression: dir\a*b??.cpp
   char *pszLastSlash = strrchr(pszIn, glblPathChar);

   #ifdef _WIN32
   const char *szAddWildCard = "*";
   #else
   const char *szAddWildCard = "";
   #endif

   if (!pszLastSlash) {
      strcpy(pszBasePath, pszIn);
      joinPath(pszPattern, lSize1+10, pszIn, (char*)szAddWildCard);
   } else {
      joinPath(pszPattern, lSize1+10, pszIn, (char*)szAddWildCard);
      strcpy(pszBasePath, pszIn);
   }

   #ifdef _WIN32 // --------- Windows directory walking code ----------

   #ifdef SFK_W64
   SFKFindData myfdat;
   intptr_t myfdh = _findfirst64(pszPattern, &myfdat);
   #else
    #ifndef _INTPTR_T_DEFINED
     typedef int intptr_t;
    #endif
   SFKFindData myfdat;
   intptr_t myfdh = _findfirst(pszPattern, &myfdat);
   #endif

   if (myfdh == -1) {
      delete [] pszPattern;
      delete [] pszBasePath;
      return 1; // yes, seems to be empty
   }

   do
   {

   #else // ----------- unix directory walking code -------------

   SFKFindData myfdat;

   DIR *myfdh = opendir(pszPattern);

   if (!myfdh) {
      delete [] pszPattern;
      delete [] pszBasePath;
      return 1;
   }

   while (1)
   {
      struct dirent *e = readdir(myfdh);
      if (e == NULL)
         break; // while

      memset(&myfdat, 0, sizeof(myfdat));
      myfdat.name   = e->d_name;
      myfdat.attrib = 0;

   #endif // _WIN32
 
      char *pszFile = myfdat.name;

      if (   !strcmp(pszFile, ".")
          || !strcmp(pszFile, ".."))
         continue;

      // first "real" file reached
      bRC = 0;
      break;
   }
   #ifdef _WIN32
    #ifdef SFK_W64
    while (!_findnext64(myfdh, &myfdat));
    #else
    while (!_findnext(myfdh, &myfdat));
    #endif
   _findclose(myfdh);
   #else
   closedir(myfdh);
   #endif

   delete [] pszPattern;
   delete [] pszBasePath;

   if (cs.verbose >= 2)
      printf("%d = isEmptyDir(%s)\n", bRC, pszIn);

   return bRC;
}

// - - - SFK primitive function library end

// internal support functions

bool startsLikeSnapFile(char *psz) {
   return strBegins(psz, ":snapfile sfk,") || strBegins(psz, ":cluster sfk,");
}

bool startsLikeActFile(char *psz) {
   return strBegins(psz, "<interleaved-attributes-text version=\"1.0\" prefix=\"\">");
}

num calcSumTime(num nRawTime, int nTimeDiff, char *pszInfo)
{
   // does RawTime point into a summertime section?
   mytime_t ntmod = (mytime_t)nRawTime;
   tm *ptmod = mylocaltime(&ntmod);
   if (ptmod && ptmod->tm_isdst) {
      // YES: change by one hour
      nRawTime += (num)nTimeDiff; // seconds
      if (pszInfo)
         printf("%s time adapted, %02d.%02d., %d\n",pszInfo,ptmod->tm_mday,ptmod->tm_mon+1,nTimeDiff);
   }
   if (ptmod && !ptmod->tm_isdst) {
      if (pszInfo)
         printf("%s time not adapted, %02d.%02d., %d\n",pszInfo,ptmod->tm_mday,ptmod->tm_mon+1,nTimeDiff);
   }
   return nRawTime;
}

#ifdef _WIN32
bool isSystemInDST()
{
   mytime_t n = (mytime_t)time(0);
   struct tm *loctm = mylocaltime(&n);
   if (loctm)
      return loctm->tm_isdst;
   return 0;
}

int makeWinFileTime(num nsrctime, FILETIME &rdsttime, num nSrcNanoSec, bool bUTC)
{
   mytime_t ftime = (mytime_t)nsrctime;
   struct tm *loctm;
   SYSTEMTIME st;
   FILETIME locft, modft;

   if (bUTC)
      loctm = mygmtime(&ftime);
   else
      loctm = mylocaltime(&ftime);

   if (loctm == NULL)
      return 9+perr("cannot convert time %s (%u)\n",numtoa(nsrctime),(uint)GetLastError());

   st.wYear         = (WORD)loctm->tm_year + 1900;
   st.wMonth        = (WORD)loctm->tm_mon + 1;
   st.wDayOfWeek    = (WORD)loctm->tm_wday;
   st.wDay          = (WORD)loctm->tm_mday;
   st.wHour         = (WORD)loctm->tm_hour;
   st.wMinute       = (WORD)loctm->tm_min;
   st.wSecond       = (WORD)loctm->tm_sec;
   // loctm->tm_isdst: no field in SYSTEMTIME
   st.wMilliseconds = 0;

   if (cs.debug) printf("mwft using time %02u:%02u\n", st.wHour, st.wMinute);

   if (bUTC)
   {
      SystemTimeToFileTime(&st, &modft);
   }
   else
   {
      if (!SystemTimeToFileTime(&st, &locft))
         return 9+perr("cannot convert time %s (%u)\n",numtoa(nsrctime),(uint)GetLastError());

      #if _MSC_VER >= 1900 // visual c++ 14.0

      /*
         Unexpected behaviour introduced with Visual 2015:

         LocalFileTimeToFileTime looks at the current DST of the PC,
           no matter which date locft represents.

         So when converting 2020-01-01 12:01:01 during SUMMER
           it will subtract one hour, while when doing this
           in WINTER it will NOT subtract one hour.

         This causes time chaos with touch, zip/unzip, sft etc.
         so it must be fixed. Of course we want to take the
         TIME ZONE of the PC into account, but never the
         (random) DST of the PC.
      */

      bool bFileTimeHasDST = loctm->tm_isdst;
      bool bThisPCHasDST   = isSystemInDST();

      num  ntime = fileTimeToTimeT(&locft);

      if (bFileTimeHasDST == 0 && bThisPCHasDST == 0) { }
      if (bFileTimeHasDST == 0 && bThisPCHasDST == 1) { ntime += 3600; }
      if (bFileTimeHasDST == 1 && bThisPCHasDST == 0) { ntime -= 3600; }
      if (bFileTimeHasDST == 1 && bThisPCHasDST == 1) { }

      timetToFileTime(ntime, &locft);

      #endif

      if (!LocalFileTimeToFileTime(&locft, &modft))
         return 9+perr("cannot convert time %s (%u)\n",numtoa(nsrctime),(uint)GetLastError());

      if (cs.debug) printf("mwft winft 0x%08x%08x\n", modft.dwHighDateTime, modft.dwLowDateTime);
   }

   if (nSrcNanoSec > 0)
   {
      modft.dwLowDateTime += (nSrcNanoSec / 100);
   }

   rdsttime = modft;

   return 0;
}
#endif

FileStat::FileStat()
{
   reset();
}

int FileStat::setFilename(char *psz)
{
   strcopy(szClFileName, psz);
   return 0;
}

char *FileStat::filename( )
{
   return szClFileName;
}

int FileStat::copyFrom(FileStat &osrc)
{
   memcpy(this, &osrc, sizeof(*this));
   return 0;
}

num FileStat::getWinTime() {
   num nval = 0;
   #ifdef _WIN32
   if (sizeof(nval) == sizeof(src.ftMTime))
      memcpy(&nval, &src.ftMTime, sizeof(nval));
   #endif
   return nval;
}

char *FileStat::attrStr() {
   memset(szClAttrStr, 0, sizeof(szClAttrStr));
   //                   01234
   strcpy(szClAttrStr, "---- ");
   if (src.bIsReadable)  szClAttrStr[0] = 'a';
   if (src.bIsWriteable) szClAttrStr[1] = 'w';
   #ifdef _WIN32
   // if (src.nAttribs & FILE_ATTRIBUTE_READONLY)   szClAttrStr[0] = 'R';
   if (src.nAttribs & FILE_ATTRIBUTE_HIDDEN)     szClAttrStr[2] = 'h';
   if (src.nAttribs & FILE_ATTRIBUTE_SYSTEM)     szClAttrStr[3] = 's';
   // if (src.nAttribs & FILE_ATTRIBUTE_ARCHIVE)    szClAttrStr[4] = 'a';
   if (src.nAttribs) szClAttrStr[4] = '-';
   #endif
   return szClAttrStr;
}

uchar *FileStat::marshal(int &nRetSize) {
   nRetSize = sizeof(src);
   return (uchar*)&src;
}

int FileStat::setFrom(uchar *pBuf, int nBufSize) {
   if (nBufSize != sizeof(src))
      return 9+perr("internal 612112005\n");
   memcpy(&src, pBuf, sizeof(src));
   return 0;
}

int FileStat::dumpTimeDiff(FileStat &rdst)
{
   num nsec = src.nMTime - rdst.src.nMTime;
   printf("diff: mtime %s sec ",numtoa(nsec));
   num nhours = nsec / 3600;
   printf("(= %s hours)",numtoa(nhours));

   #ifdef _WIN32
   num nSrcWFT = 0;
   num nDstWFT = 0;
   if (sizeof(nSrcWFT) == sizeof(src.ftMTime)) {
      memcpy(&nSrcWFT, &src.ftMTime, sizeof(nSrcWFT));
      memcpy(&nDstWFT, &rdst.src.ftMTime, sizeof(nDstWFT));
      num nxnano = nSrcWFT - nDstWFT;
      num nmsec  = nxnano / 10;
      num nwsec  = nmsec / 1000000;
      num nwhours = nwsec / 3600;
      printf(", wft mtime %s usec*10 ", numtoa(nxnano));
      printf(" (= %s sec, ", numtoa(nwsec));
      printf("or %s hours)",numtoa(nwhours));
   }
   #endif

   printf("\n");

   nsec = src.nCTime - rdst.src.nCTime;
   printf("      ctime %s sec ",numtoa(nsec));
   nhours = nsec / 3600;
   printf("(= %s hours)",numtoa(nhours));

   #ifdef _WIN32
   if (sizeof(nSrcWFT) == sizeof(src.ftCTime)) {
      memcpy(&nSrcWFT, &src.ftCTime, sizeof(nSrcWFT));
      memcpy(&nDstWFT, &rdst.src.ftCTime, sizeof(nDstWFT));
      num nxnano = nSrcWFT - nDstWFT;
      num nmsec  = nxnano / 10;
      num nwsec  = nmsec / 1000000;
      num nwhours = nwsec / 3600;
      printf(", wft ctime %s usec*10 ", numtoa(nxnano));
      printf(" (= %s sec, ", numtoa(nwsec));
      printf("or %s hours)",numtoa(nwhours));
   }
   #endif

   printf("\n");

   return 0;
}

int FileStat::dump()
{
   char szBuf1[100],szBuf2[100],szBuf3[100];
   char szHead[300];
 
   uint nutc = (cs.timemask & 1024);

   if (cs.timemask & 512)
   {
      dumpSub(0, 16+512+nutc, szBuf1, sizeof(szBuf1));
      dumpSub(1, 16+512+nutc, szBuf2, sizeof(szBuf2));
      #ifdef _WIN32
      dumpSub(2, 16+512+nutc, szBuf3, sizeof(szBuf3));
      #endif
 
      cchar *pprem = cs.tabform ? "":"mtime ";
      cchar *pprea = cs.tabform ? "\t":", atime ";
      cchar *pprec = cs.tabform ? "\t":", ctime ";
      cchar *ppos  = cs.tabform ? "\t":", ";

      if (!cs.tabform && !(cs.timemask & 8)) {
         pprem = "";
         pprea = "";
         pprec = "";
         ppos  = " ";
      }

      szHead[0] = '\0';

      if (cs.timemask & 1) { strcat(szHead,pprem); strcat(szHead,szBuf1); }
      if (cs.timemask & 2) { strcat(szHead,pprea); strcat(szHead,szBuf2); }
      #ifdef _WIN32
      if (cs.timemask & 4) { strcat(szHead,pprec); strcat(szHead,szBuf3); }
      #endif

      if (cs.timemask & 256)
         chain.print("%s%s%s\n", szHead, ppos, szClFileName);
      else
         chain.print("%s\n", szHead);
   }
   else
   {
      if (cs.timemask & 1) dumpSub(0, cs.timemask, 0, 0);
      if (cs.timemask & 2) dumpSub(1, cs.timemask, 0, 0);
      #ifdef _WIN32
      if (cs.timemask & 4) dumpSub(2, cs.timemask, 0, 0);
      #endif
   }

   return 0;
}

int FileStat::dumpSub(int nRow, uint nmask, char *pszOut, int iMaxOut)
{
   char szLocDate[100]; szLocDate[0] = '\0';
   char szLocTime[100]; szLocTime[0] = '\0';
   char szUTCTime[100]; szUTCTime[0] = '\0';
   char szUXTime [100]; szUXTime[0]  = '\0';
   char szWFTTime[100]; szWFTTime[0] = '\0';
   char szBuf[200]; szBuf[0] = '\0';

   num nTime = 0;
   cchar *pszPrefix = "";

   switch (nRow)
   {
      case 0: nTime = src.nMTime; pszPrefix = "mtime"; break;
      case 1: nTime = src.nATime; pszPrefix = "atime"; break;
      case 2: nTime = src.nCTime; pszPrefix = "ctime"; break;
   }

   mytime_t tTime = (mytime_t)nTime;

   struct tm *pltime = 0;

   if (nmask & 1024)
      pltime = mygmtime(&tTime);
   else
      pltime = mylocaltime(&tTime);

   if (pltime) {
      if (cs.flatTime) {
         strftime(szLocDate, sizeof(szLocDate)-10, "%Y%m%d", pltime);
         strftime(szLocTime, sizeof(szLocTime)-10, "%H%M%S", pltime);
      } else {
         strftime(szLocDate, sizeof(szLocDate)-10, "%Y-%m-%d", pltime);
         strftime(szLocTime, sizeof(szLocTime)-10, "%H:%M:%S", pltime);
      }
   }

   struct tm *pgtime = mygmtime(&tTime);
   if (pgtime) {
      if (cs.flatTime)
         strftime(szUTCTime, sizeof(szUTCTime)-10, "%H%M%S", pgtime);
      else
         strftime(szUTCTime, sizeof(szUTCTime)-10, "%H:%M:%S", pgtime);
   }

   numtoa(nTime, 10, szUXTime);

   #ifdef _WIN32
   num nWinFileTime = 0;
   bool bCopyDone = 0;
   switch (nRow) {
      case 0:
         if (sizeof(nWinFileTime) == sizeof(src.ftMTime)) {
            memcpy(&nWinFileTime, &src.ftMTime, sizeof(nWinFileTime));
            bCopyDone = 1;
         }
         break;
      case 1:
         if (sizeof(nWinFileTime) == sizeof(src.ftATime)) {
            memcpy(&nWinFileTime, &src.ftATime, sizeof(nWinFileTime));
            bCopyDone = 1;
         }
         break;
      case 2:
         if (sizeof(nWinFileTime) == sizeof(src.ftCTime)) {
            memcpy(&nWinFileTime, &src.ftCTime, sizeof(nWinFileTime));
            bCopyDone = 1;
         }
         break;
   }
   if (bCopyDone)
      numtoa(nWinFileTime, 18, szWFTTime);
   #endif

   cchar *ppre = "";

   int nzones = 0;
   if (nmask &  16) nzones++;
   if (nmask &  32) nzones++;
   if (nmask &  64) nzones++;
   if (nmask & 128) nzones++;

   cchar *pwhite  = cs.tabform ? "\t":" ";
   cchar *pzon    = (nzones > 1) ? ", ":pwhite;
   cchar *pcon    = cs.tabform?"\t":pzon;
   cchar *ptwhite = (cs.flatTime >= 2) ? "":pwhite;

   if (nmask &   8) { mystrcatf(szBuf, sizeof(szBuf), "%s%s"    , pszPrefix, cs.tabform?"":": "); if (cs.tabform) ppre="\t"; }
   if (nmask &  16) { mystrcatf(szBuf, sizeof(szBuf), "%s%s%s%s", ppre, szLocDate, ptwhite, szLocTime); ppre=pcon; }
   if (nmask &  32) { mystrcatf(szBuf, sizeof(szBuf), "%s%s%s"  , ppre, szUTCTime, cs.tabform?"":" utc"  ); ppre=pcon; }
   if (nmask &  64) { mystrcatf(szBuf, sizeof(szBuf), "%s%s%s"  , ppre, szUXTime , cs.tabform?"":" sec"  ); ppre=pcon; }
   #ifdef _WIN32
   if (nmask & 128) { mystrcatf(szBuf, sizeof(szBuf), "%s%s%s"  , ppre, szWFTTime, cs.tabform?"":" wft"  ); ppre=pcon; }
   #endif
   if (nmask & 256) { mystrcatf(szBuf, sizeof(szBuf), "%s%s"    , ppre, szClFileName); ppre=pcon; }

   if (nmask & 512) {
      if (!pszOut)
         return 9;
      snprintf(pszOut, iMaxOut-10, "%s", szBuf);
   } else {
      chain.print("%s\n", szBuf);
   }

   return 0;
}

void FileStat::reset()
{
   memset(this, 0, sizeof(*this));
}

int FileStat::writeStat(int iTraceLine)
{
   if (!szClFileName[0])
      return 9+perr("missing stat filename, cannot write filetime (%d)", iTraceLine);

   if (src.nMTime <= 0)
      return 10+perr("missing stat data, cannot write filetime (%d)", iTraceLine);

   return writeTo(szClFileName, iTraceLine);
}

const char *FileStat::diffReason(int nReason)
{
   if (szClDiffReason[0])
      return szClDiffReason;

   switch (nReason) {
      case  0: return "none";
      case  8: return "newer, wft";
      case  7: return "time, wft";
      case  9: return "newer, uft";
      case 10: return "time, uft";
      case  1: return "size";
      case  3: return "isreadable";
      case  4: return "iswriteable";
      case  5: return "attributes";
   }
   return "unknown";
}

/*
   Factors influencing conversion:
      1. is src on UTC or non-UTC file system
      2. is src time dependent on DST, i.e. is it different
         depending on the fact that we currently have DST or not?
      3. is src's date stamp lying within a DST zone?
      4. all above for dst as well
*/

int FileStat::writeTo(char *pszDstFile, int nTraceLine, bool bWriteJustTime)
{
   if (!pszDstFile || !pszDstFile[0])
   {
      perr("missing stat filename, cannot write file time (%d)\n", nTraceLine);
      return 9;
   }

   // totally experimental and incomplete,
   // esp. on NTFS <-> FAT file transfers.

   #ifdef _WIN32
   HANDLE hDst = CreateFile(
      pszDstFile,
      FILE_WRITE_ATTRIBUTES,
      0,    // share
      0,    // security
      OPEN_EXISTING,
      src.bIsDir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL,
      0     // template file
      );
   if (hDst == INVALID_HANDLE_VALUE) {
      uint nerr = GetLastError();
      perr("cannot set attributes (rc=%u): %s\n",nerr,pszDstFile);
      if (nerr == ERROR_ACCESS_DENIED)
        pinf("make sure you have full access rights. maybe you have to be administrator.\n");
      return 9;
   }

   int nrc = 0;

   // src.nMTime is a value in SECONDS since 1970.

   FILETIME nDstMTime, nDstCTime, nDstATime;

   // native windows file times are copied 1:1
   if (src.nHaveWFT)
   {
      memcpy(&nDstMTime, &src.ftMTime, sizeof(nDstMTime));

      if (src.nHaveWFT > 1)
         memcpy(&nDstCTime, &src.ftCTime, sizeof(nDstCTime));

      if (src.nHaveWFT > 1)
         memcpy(&nDstATime, &src.ftATime, sizeof(nDstATime));
   }
   else
   {
      if (!nrc && src.nMTime)
         if (makeWinFileTime(src.nMTime, nDstMTime, 0, src.bIsUTCTime)) // FileStat
            nrc = 9;
 
      if (!nrc && src.nCTime)
         if (makeWinFileTime(src.nCTime, nDstCTime))
            nrc = 9;
 
      if (!nrc && src.nATime)
         if (makeWinFileTime(src.nATime, nDstATime))
            nrc = 9;
   }

   #ifdef WINFULL
   if (!nrc) {
      if (!SetFileTime(hDst, &nDstCTime, &nDstATime, &nDstMTime)) {
         perr("cannot set file time (rc=%u): %s %s\n", (uint)GetLastError(), pszDstFile, numtohex(src.nMTime));
         nrc = 9;
      }
   }
   #endif
 
   CloseHandle(hDst);

   if (!bWriteJustTime)
   {
      // CreateFile() was done ONLY to set filetime.
      // the attributes require another API call:
      if (src.nAttribs == 0) {
         perr("no attributes to set for %s\n",pszDstFile);
         nrc = 9;
      }
      else
      if (!SetFileAttributes(pszDstFile, src.nAttribs)) {
         perr("failed to set attributes (rc=%u): %s %s\n", (uint)GetLastError(), pszDstFile, numtohex(src.nMTime));
         nrc = 9;
      }
   }

   return nrc;

   #else

   // linux generic

   struct utimbuf otimes;
   mclear(otimes); // FIX: 167: missing actime init
   if (src.nATime)
      otimes.actime = src.nATime;
   otimes.modtime = src.nMTime;
   int iRC = utime(pszDstFile, &otimes);
   if (iRC) return 9+perr("failed to set file times (rc=%d): %s\n",iRC,pszDstFile);

   if (!bWriteJustTime)
   {
      mode_t nmode = (mode_t)src.nAttribs;
      if (nmode == 0) return 9+perr("no attributes to set for %s\n",pszDstFile);
      iRC = chmod(pszDstFile, nmode);
      if (iRC) return 9+perr("failed to set file attributes (rc=%d): %s\n",iRC,pszDstFile);
   }

   return 0;

   #endif
}

bool is3600Range(num n) {
   if (n >= 3598 && n <= 3602)
      return true;
   return false;
}

/*
   RC 1 ...  5: difference in attributes
   RC 7 ... 10: difference in time
*/
int FileStat::differs(FileStat &oref, bool bSameIfOlderSrc, bool *pSrcIsOlder)
{
   int nTimeState = 0; // 0: time is no reason to process

   szClDiffReason[0] = '\0';

   #ifdef _WIN32
   if (src.nHaveWFT && oref.src.nHaveWFT)
   {
      // compare windows filetime, to be safer on DST switches and time zones.
      // NOTE: files from FAT filesystems will also produce a pseudo wft
      // (calculated implicitely by the OS) that will still jump on DST.

      num nSrcMTime = 0;
      num nRefMTime = 0;
      memcpy(&nSrcMTime, &src.ftMTime, sizeof(nSrcMTime));
      memcpy(&nRefMTime, &oref.src.ftMTime, sizeof(nRefMTime));

      num nSrcSec   = fileTimeToTimeT(nSrcMTime);
      num nNowSec   = getSystemTime();
      num nAgeSec   = nNowSec - nSrcSec;
      int nAgeDays = (int)(nAgeSec / (24 * 3600));

      num ndif = (nSrcMTime - nRefMTime) / 10000000; // in seconds
      if (abs((int)ndif) < 3)
         nTimeState = 0; // time is no reason
      else
      if (bGlblIgnore3600 && is3600Range(abs((int)ndif)) && (nAgeDays > nGlblActiveFileAgeLimit))
      {
         nTimeState = 0; // dst jump difference
      }
      else
      if (bSameIfOlderSrc) {
         if (cs.verbose > 1 && (ndif != 0))
            printf("wft diff %d [1]\n", ndif);
         if (ndif > 0) {
            sprintf(szClDiffReason, "newer, wft by %s sec", numtoa(ndif));
            nTimeState = 8; // src is newer, must process
         }
         if ((ndif < 0) && pSrcIsOlder) {
            if (is3600Range(abs((int)ndif)))
               *pSrcIsOlder = 2; // notice: probably older due to dst jump
            else
               *pSrcIsOlder = 1; // notice
         }
      } else {
         if (cs.verbose > 1 && (ndif != 0))
            printf("wft diff %d [2]\n", ndif);
         if (nRefMTime != nSrcMTime) {
            sprintf(szClDiffReason, "time, wft by %s sec", numtoa(ndif));
            nTimeState = 7; // any time diff, must process
         }
         if ((ndif < 0) && pSrcIsOlder)
            *pSrcIsOlder = 1; // just a notice
      }
      // else fall through, time check passed
   }
   else
   #endif
   if (src.nMTime != oref.src.nMTime)
   {
      // a time difference: ignore below a threshold
      //   NTFS : 100 nanoseconds resolution
      //   Linux:   1 second resolution
      //   FAT32:   2 seconds resolution
      // therefore all differences below 3 seconds are ignored.

      num nSrcSec   = src.nMTime;
      num nNowSec   = getSystemTime();
      num nAgeSec   = nNowSec - nSrcSec;
      int nAgeDays = (int)(nAgeSec / (24 * 3600));

      num ndif = src.nMTime - oref.src.nMTime;
      if (abs((int)ndif) < 3)
         nTimeState = 0; // time is no reason
      else
      if (bGlblIgnore3600 && is3600Range(abs((int)ndif)) && (nAgeDays > nGlblActiveFileAgeLimit))
      {
         nTimeState = 0; // dst jump difference
      }
      else
      if (bSameIfOlderSrc) {
         if (cs.verbose > 1 && (ndif != 0))
            printf("wft diff %d [3]\n", (int)ndif);
         if (ndif > 0) {
            sprintf(szClDiffReason, "newer, uft by %s sec", numtoa(ndif));
            nTimeState = 9; // src is newer, must process
         }
         if ((ndif < 0) && pSrcIsOlder) {
            if (is3600Range(abs((int)ndif)))
               *pSrcIsOlder = 2; // notice: probably older due to dst jump
            else
               *pSrcIsOlder = 1; // notice
         }
      }
      else {
         if (cs.verbose > 1 && (ndif != 0))
            printf("wft diff %d [4]\n", (int)ndif);
         sprintf(szClDiffReason, "time, uft by %s sec", numtoa(ndif));
         nTimeState = 10; // any time diff, must process
         if ((ndif < 0) && pSrcIsOlder)
            *pSrcIsOlder = 1; // just a notice
      }
   }

   // with "sameifolder", processing is based only on timestamp.
   if (bSameIfOlderSrc)
   {
      // if time is equal, don't process , no matter if size etc. differs!
      // if src  is older, don't process , no matter if size etc. differs!
      // if src  is newer, always process, no matter if size etc. is equal!
      return nTimeState;
   }

   // else process on size or attribute difference.
   if (src.bIsDir != oref.src.bIsDir   // sfk1972 filestat mark dir vs file diff as size
       || (src.bIsDir == 0             // sfk1972 filestat ignore 'size' diff on folder objects,
           && src.nSize != oref.src.nSize)   // as these cannot be cloned, across disks.
      )
   {
      if (cs.verbose > 1)
         printf("[size differs, %u %u dir=%u/%u]\n",(uint)src.nSize,(uint)oref.src.nSize,
            src.bIsDir, oref.src.bIsDir);
      return 1;
   }

   if (src.bIsReadable  != oref.src.bIsReadable ) return 3;
   if (src.bIsWriteable != oref.src.bIsWriteable) return 4;

   #ifdef _WIN32
   if (   (src.nAttribs      & WINFILE_ATTRIB_MASK)
       != (oref.src.nAttribs & WINFILE_ATTRIB_MASK)
      )
   {
      if (cs.verbose > 1)
         printf("[win_attributes differ, %x %x]\n",src.nAttribs,oref.src.nAttribs);
      return 5;
   } else {
      if (cs.verbose > 2)
         printf("[win_attributes match, %x %x]\n",src.nAttribs,oref.src.nAttribs);
   }
   #endif

   return nTimeState;
}

// no vname support so far, due to getFileStat
int FileStat::readFrom(char *pszSrcFile, bool bWithFSInfo, bool bSilent)
{
   reset();

   sfkstat_t ostat;

   szClFileName[0] = '\0'; // safety

   num aExtTimes[2];
   mclear(aExtTimes);
   if (getFileStat(pszSrcFile, src.bIsDir, src.bIsReadable, src.bIsWriteable, src.nMTime, src.nSize, aExtTimes, &ostat, sizeof(ostat)))
      return 10;

   strcopy(szClFileName, pszSrcFile);

   src.nCTime = aExtTimes[0];
   src.nATime = aExtTimes[1];

   #if (_MSC_VER >= 1900)
   src.bIsUTCTime = 1; // FIX sfk197 vc14
   #else
   src.bIsUTCTime = 0;
   #endif

   num   nTotalBytes = 0;
   num   nFreeBytes  = 0;

   if (bWithFSInfo)
   {
      // must extract path from pszSrcFile. if there is none, use ".".
      mystrcopy(szClSrcPath, pszSrcFile, sizeof(szClSrcPath)-10);
 
      // turn the/dir into the/dir/
      if (src.bIsDir && strlen(szClSrcPath)>0)
         if (szClSrcPath[strlen(szClSrcPath)-1] != glblPathChar)
            strcat(szClSrcPath,glblPathStr);
 
      #ifdef _WIN32
      // windows only: if path starts with c: ...
      if (strlen(szClSrcPath) >= 2 && szClSrcPath[1] == ':') {
         // then reduce c:\whatever to c:\, extend c: to c:/
         szClSrcPath[2] = glblPathChar;
         szClSrcPath[3] = '\0';
      }
      else
      if (!strncmp(szClSrcPath, "\\\\", 2)) {
         // network path
      }
      else {
         // relative path: reduce to "\\", should be same volume.
         // will probably NOT work with LINKS.
         strcpy(szClSrcPath, "\\");
      }
      #endif
 
      // try for x:\thefile, \thefile, the\path\file
      char *psz1 = strrchr(szClSrcPath, glblPathChar);
      if (psz1 && (psz1 > szClSrcPath)) {
         psz1++;
         *psz1 = '\0';
      } else {
         #ifdef _WIN32
         // try for x:thefile
         psz1 = strchr(szClSrcPath, ':');
         if (psz1 && (psz1 > szClSrcPath)) {
            // change x:thefile to x:/
            psz1++;
            *psz1++ = glblPathChar;
            *psz1   = '\0';
         }
         else
            strcpy(szClSrcPath, "\\");
         #else
         // TODO: use /, ./ or "." with linux?
         strcpy(szClSrcPath, ".");
         #endif
      }

      if (cs.debug)
         printf("filestat: \"%s\" => \"%s\"\n",pszSrcFile,szClSrcPath);

      uint nVolID = 0;
      if (getFileSystemInfo(szClSrcPath, nTotalBytes, nFreeBytes,
         szClSrcFSName, sizeof(szClSrcFSName)-10,
         szClSrcVolID , sizeof(szClSrcVolID)-10,
         nVolID
         ))
         return 11;
 
      #ifdef _WIN32
      if (cs.debug)
         printf("\"%s\" => %s, %s\n",szClSrcPath, szClSrcFSName, szClSrcVolID);
      #endif
   }

   #ifdef _WIN32

   #ifdef WINFULL
   WIN32_FILE_ATTRIBUTE_DATA oinf;
   BOOL bok2 = 0;
   if (vname()) {
      sfkname oname(pszSrcFile);
      bok2 = GetFileAttributesExW((const wchar_t *)oname.wname(), GetFileExInfoStandard, &oinf);
   } else {
      bok2 = GetFileAttributesEx(pszSrcFile, GetFileExInfoStandard, &oinf);
   }
   if (!bok2) {
      src.nAttribs = 0;
      if (!bSilent)
         perr("cannot read attributes: %s (rc %u)\n", pszSrcFile, GetLastError());
      return 12;
   }
   src.nAttribs = oinf.dwFileAttributes;
   // => see WINFILE_ATTRIB_MASK
   // ftCreationTime; ftLastAccessTime; ftLastWriteTime
   memcpy(&src.ftMTime, &oinf.ftLastWriteTime , sizeof(src.ftMTime));
   memcpy(&src.ftCTime, &oinf.ftCreationTime  , sizeof(src.ftCTime));
   memcpy(&src.ftATime, &oinf.ftLastAccessTime, sizeof(src.ftATime));
   src.nHaveWFT = 2;
   #else
   src.nAttribs = GetFileAttributes(pszSrcFile);
   #endif

   #else

   // src.bIsReadable, src.bIsWriteable
   src.nAttribs = (uint)ostat.st_mode;

   #endif

   return 0;
}

int cloneAttributes(char *pszSrc, char *pszDst, int nTraceLine)
{
   FileStat ofs;
   if (ofs.readFrom(pszSrc)) return 9;
   if (ofs.writeTo(pszDst, nTraceLine)) return 9;
   return 0;
}

int getFileMD5(Coi *pcoi, SFKMD5 &md5, bool bSilent, bool bInfoCycle)
{
   if (pcoi->open("rb")) {
      if (!bSilent) pferr(pcoi->name(), "cannot read: %s\n", pcoi->name());
      return 9;
   }
   size_t nRead = 0;
   while ((nRead = pcoi->read(abBuf,sizeof(abBuf)-10)) > 0) {
      md5.update(abBuf,nRead);
      cs.totalbytes += nRead;
      if (bInfoCycle)
         info.cycle();
   }
   pcoi->close();
   return 0;
}

int getFileMD5(char *pszFile, SFKMD5 &md5, bool bSilent, bool bInfoCycle)
{
   FILE *fin = fopen(pszFile, "rb");
   if (!fin) {
      if (!bSilent) pferr(pszFile, "cannot read: %s\n", pszFile);
      return 9;
   }
   size_t nRead = 0;
   while ((nRead = fread(abBuf,1,sizeof(abBuf)-10,fin)) > 0) {
      md5.update(abBuf,nRead);
      cs.totalbytes += nRead;
      if (bInfoCycle)
         info.cycle();
   }
   fclose(fin);
   return 0;
}

int getFileMD5(char *pszFile, uchar *abOut16)
{
   FILE *fin = fopen(pszFile, "rb");
   if (!fin) return 9;
   SFKMD5 md5;
   size_t nRead = 0;
   while ((nRead = fread(abBuf,1,sizeof(abBuf)-10,fin)) > 0) {
      md5.update(abBuf,nRead);
      cs.totalbytes += nRead;
   }
   fclose(fin);
   uchar *pmd5 = md5.digest();
   for (uint k=0; k<16; k++)
      abOut16[k] = pmd5[k];
   return 0;
}

#ifdef _WIN32
// size of windows-specific read buffer must be
// a multiple of disk's sector size. the following
// should be sufficient for everything.
int   nGlblMD5NoCacheBufSize = 128000;
LPVOID pGlblMD5NoCacheBuf     = 0;
#endif

int getFileMD5NoCache(char *pszFile, uchar *abOut16, bool bSilent)
{
   #ifdef _WIN32
   int lRC = 0;

   int nBufSize = nGlblMD5NoCacheBufSize;

   // alloc read buffer on demand
   if (pGlblMD5NoCacheBuf == 0) {
      pGlblMD5NoCacheBuf = VirtualAlloc(0, nBufSize, MEM_COMMIT, PAGE_READWRITE);
      if (!pGlblMD5NoCacheBuf) return 11;
   }

   LPVOID pBuf = pGlblMD5NoCacheBuf;

   /*
   DWORD nBytesPerSector = 0;
   DWORD nDummy1, nDummy2, nDummy3;
   // try to get sector size
   if (GetDiskFreeSpace(szRootDir, &nDummy1, &nBytesPerSector, &nDummy2, &nDummy3)) {
      // adjust max. read size to multiples of sector size
      if (nBytesPerSector > 0) {
         int nBufSize2 = (int)(nBufSize / nBytesPerSector) * nBytesPerSector;
         printf("adjusted %d => %d by %d\n",nBufSize,nBufSize2,nBytesPerSector);
         nBufSize = nBufSize;
      }
   } else {
      printf("GetDiskFreeSpace failed\n");
   }
   */

   HANDLE hDst = CreateFile(
      pszFile,
      FILE_READ_DATA,
      0,    // share
      0,    // security
      OPEN_EXISTING,
      FILE_FLAG_NO_BUFFERING,
      0     // template file
      );

   if (hDst == INVALID_HANDLE_VALUE)
      return 9;

   SFKMD5 md5;
   DWORD nRead = 0;
   while (true) {
      nRead = 0;
      if (!ReadFile(hDst, pBuf, nBufSize, &nRead, 0))
         {  lRC = 10; break;   }
      if (nRead <= 0)
         break;
      // printf("%d bytes,\n\"%.*s\"\n",nRead,(int)nRead,pBuf);
      md5.update((uchar*)pBuf,nRead);
      cs.totalbytes += nRead;
      if (userInterrupt(bSilent))
         {  lRC = 8; break;   }
      info.cycle();
   }

   CloseHandle(hDst);

   if (lRC == 10 && GetLastError() == 87) {
      // ReadFile did not accept buffer size. try fallback:
      // pwarn("md5nocache: readfile failed, using fallback\n");
      return getFileMD5(pszFile, abOut16);
   }

   uchar *pdigest = md5.digest();
   memcpy(abOut16, pdigest, 16);

   return lRC;
   #else
   FILE *fin = fopen(pszFile, "rb");
   if (!fin) return 9;
   SFKMD5 md5;
   size_t nRead = 0;
   while ((nRead = fread(abBuf,1,sizeof(abBuf)-10,fin)) > 0) {
      md5.update(abBuf,nRead);
      cs.totalbytes += nRead;
      if (userInterrupt(bSilent)) {
         fclose(fin);
         return 8;
      }
      info.cycle();
   }
   fclose(fin);
   uchar *pmd5 = md5.digest();
   for (uint k=0; k<16; k++)
      abOut16[k] = pmd5[k];
   return 0;
   #endif
}

#ifdef _WIN32
// type: 1==onlyDown 2==onlyUp 0==any
int getKeyPress(int ntype=0)
{
   #ifdef WINFULL
   DWORD dwNumEvents, dwEventsPeeked, dwInputEvents;
   INPUT_RECORD aInputBuffer[1];

   HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

   if (!GetNumberOfConsoleInputEvents(hStdIn, &dwNumEvents))
      return -1;

   if (dwNumEvents <= 0)
      return -1;

   if (!PeekConsoleInput(hStdIn, aInputBuffer, 1, &dwEventsPeeked))
      return -1;
 
   if (!ReadConsoleInput(hStdIn, &aInputBuffer[0], 1, &dwInputEvents))
      return -1;

   if (dwInputEvents != 1)
      return -1;

   if (aInputBuffer[0].EventType == KEY_EVENT) {
      int ncode = aInputBuffer[0].Event.KeyEvent.wVirtualKeyCode;
      bool bdown = aInputBuffer[0].Event.KeyEvent.bKeyDown;
      // react only on key down or key up?
      if (ntype == 1 && !bdown) return -1; // no type match
      if (ntype == 2 &&  bdown) return -1; // no type match
      return ncode;
   }
   #endif

   return -1;
}

bool userInterrupt(bool bSilent, bool bWaitForRelease)
{
   if (bGlblEscape)
      return 1;
   static bool bTold = 0;
   int nKey = getKeyPress(1);
   if (bGlblDisableEscape == 0 && nKey == VK_ESCAPE)
   {
      if (!bSilent && !bTold) {
         bTold = 1;
         info.clear(); // if any
         printf("[stopped by user]%.50s\n", pszGlblBlank);
      }
      if (bWaitForRelease)
         while (getKeyPress() == VK_ESCAPE)
            doSleep(200);
      bGlblEscape = 1; // on esc key
      return 1;
   }
   if (nKey == '\r')
   {
      if (!bSilent) {
         num nTime = mytime(NULL);
         mytime_t nTime2 = (mytime_t)nTime;
         struct tm *pLocTime = mylocaltime(&nTime2);
         if (pLocTime) {
            char szBuf[100+10];
            strftime(szBuf, 100, "%H:%M:%S", pLocTime);
            printf("--- %s ---   \n", szBuf);
         } else {
            printf("---\n");
         }
      }
      bGlblEnter = 1;
   }
   return 0;
}
#else
bool userInterrupt(bool bSilent, bool bWait)
{
   // FIX 1642: always returned 0
   return bGlblEscape;
}
#endif

void padBuffer(char *pszBuf, int nMaxLen, char c, int nTargLen)
{
   int nlen = strlen(pszBuf);
   if (nlen < nTargLen) {
      int ndif = nTargLen-nlen;
      if (nlen+ndif < nMaxLen) {
         memset(pszBuf+nlen, c, ndif);
         pszBuf[nlen+ndif] = '\0';
      }
   }
}

bool isRemarkStart(char *psz) // like !strncmp(ptok, "//", 2) but with space check
{
   if (strncmp(psz, "//", 2)) return 0;
   if (isspace(psz[2])) return 1;
   // sfk197 deprecation of "//" remarks without whitespace
   static bool btold=0;
   if (btold) return 1;
   btold=1;
   pwarn("missing space after '//' remark is deprecated: %.32s\n", dataAsTrace(psz));
   return 1;
}

char szSafeRunBuf[MAX_LINE_LEN+10]; // sfk1972

// change "mycmd" to "mycmd 2>&1".
char *safeRunCommand(char *psz) {
   if (strstr(psz, " 2>"))
      return psz;
   mystrcopy(szSafeRunBuf, psz, MAX_LINE_LEN);
   strcat(szSafeRunBuf, " 2>&1");
   return szSafeRunBuf;
}

UTF8Codec::UTF8Codec(char *pOptInData, int iOptionalInputLength)
{
   memset(this, 0, sizeof(*this));

   if (pOptInData)
      init(pOptInData, iOptionalInputLength);
}

void UTF8Codec::init(char *p, int ilen)
{
   icur = 0;
   psrc = (uchar*)p;

   if (ilen < 0)
      imax = strlen(p);
   else
      imax = ilen;
}

int UTF8Codec::readRaw()
{
   if (icur >= imax)
      return 0;
   return psrc[icur++] & 0xFF;
}

int UTF8Codec::readSeq()
{
   int c = readRaw();
   return ((c & 0xC0) == 0x80) ? (c & 0x3F) : -1;
}

bool UTF8Codec::hasChar() { return (icur < imax) ? 1 : 0; }

bool UTF8Codec::eod() { return (icur >= imax) ? 1 : 0; }

int  UTF8Codec::validSeqLen(char *pszSrc, int iMaxSrc)
{
   UTF8Codec obj;
   return obj.validSeqLenInt(pszSrc, iMaxSrc);
}

int  UTF8Codec::validSeqLenInt(char *pszSrc, int iMaxSrc)
{
   if (iMaxSrc < 2)
      return 0;

   init(pszSrc, iMaxSrc);

   if (icur >= imax)
        return 0;

   int c = readRaw();

   int iold = icur;

   if ((c & 0x80) == 0)
      return 0;

   int c1,c2,c3,n;

   if ((c & 0xE0) == 0xC0) {
      if ((c1 = readSeq()) < 0)
         return 0;
      n = ((c & 0x1F) << 6) | c1;
      if (n >= 128)
         return 2;
      return 0;
   } else if ((c & 0xF0) == 0xE0) {
      if ((c1 = readSeq()) < 0)
         return 0;
      if ((c2 = readSeq()) < 0)
         return 0;
      n = ((c & 0x0F) << 12) | (c1 << 6) | c2;
      if (n >= 0x800 && (n < 0xD800 || n > 0xDFFF))
         return 3;
      return 0;
   } else if ((c & 0xF8) == 0xF0) {
      if ((c1 = readSeq()) < 0)
         return 0;
      if ((c2 = readSeq()) < 0)
         return 0;
      if ((c3 = readSeq()) < 0)
         return 0;
      return 4;
   }

   return 0;
}

uint UTF8Codec::nextChar()
{
   if (icur >= imax)
      return 0;

   if (bdecodexml==1 && psrc[icur]=='&')
   {
      // decode xml predefined entities
      int irem=imax-icur;
      if (irem>=6 && !strncmp((char*)psrc+icur, "&quot;", 6))
        { icur+=6; return '\"'; }
      if (irem>=6 && !strncmp((char*)psrc+icur, "&apos;", 6))
        { icur+=6; return '\''; }
      if (irem>=5 && !strncmp((char*)psrc+icur, "&amp;", 5))
        { icur+=5; return '&'; }
      if (irem>=4 && !strncmp((char*)psrc+icur, "&lt;", 4))
        { icur+=4; return '<'; }
      if (irem>=4 && !strncmp((char*)psrc+icur, "&gt;", 4))
        { icur+=4; return '>'; }
      icur++;
      return '&';
   }

   int c = readRaw();

   int iold = icur;

   if (bkeeputf == 1 || (c & 0x80) == 0)
      return c;

   banychars = 1;

   int c1,c2,c3,n;

   do {
      if ((c & 0xE0) == 0xC0) {
         if ((c1 = readSeq()) < 0)
            break;
         n = ((c & 0x1F) << 6) | c1;
         if (n >= 128)
            return n;
      } else if ((c & 0xF0) == 0xE0) {
         if ((c1 = readSeq()) < 0)
            break;
         if ((c2 = readSeq()) < 0)
            break;
         n = ((c & 0x0F) << 12) | (c1 << 6) | c2;
         if (n >= 0x800 && (n < 0xD800 || n > 0xDFFF))
            return n;
      } else if ((c & 0xF8) == 0xF0) {
         if ((c1 = readSeq()) < 0)
            break;
         if ((c2 = readSeq()) < 0)
            break;
         if ((c3 = readSeq()) < 0)
            break;
         return (((c & 0x0F) << 18) | (c1 << 12) | (c2 << 6) | c3) + 0x10000;
      }
   }
   while (0);

   // invalid sequence:
   bbadchars = 1;

   // return undecoded text
   icur = iold;

   return c;
}

bool UTF8Codec::isValidUTF8(char *psz)
{
   UTF8Codec utf(psz);

   while (utf.nextChar());

   if (utf.banychars==0)
      return 0;
   if (utf.bbadchars==1)
      return 0;

   return 1;
}

int UTF8Codec::toutf8(char *pszOut, int iMaxOut, uint ch)
{
   uint c = ch;
   int len = 0;
   uint first = 0;
 
   if (c < 0x80) {
      first = 0;
      len = 1;
   }
   else if (c < 0x800) {
      first = 0xc0;
      len = 2;
   } else if (c < 0x10000) {
      first = 0xe0;
      len = 3;
   } else if (c < 0x200000) {
      first = 0xf0;
      len = 4;
   } else if (c < 0x4000000) {
      first = 0xf8;
      len = 5;
   } else {
      first = 0xfc;
      len = 6;
   }

   if (len >= iMaxOut)
      return 0;

   if (!pszOut)
      return 0;

   for (int i = len - 1; i > 0; i--)
   {
      pszOut[i] = (char)((c & 0x3f) | 0x80);
      c >>= 6;
   }
   pszOut[0] = c | first;
 
   return len;
}

int UTF8Codec::toutf8(char *pszOut, int iMaxOut, char *pszSrc, bool bSafe)
{
   if (iMaxOut < 2)
      return 0;

   *pszOut = '\0';

   char *pDstCur = pszOut;
   char *pDstMax = pszOut+iMaxOut;
   int   iSrcLen = strlen(pszSrc);

   uchar *pSrcCur = (uchar*)pszSrc;
   uchar *pSrcMax = pSrcCur + iSrcLen;

   UTF8Codec obj;

   while (pSrcCur<pSrcMax && *pSrcCur != 0)
   {
      if (bSafe) {
         // skip existing utf8 sequences in mixed text
         int l = obj.validSeqLenInt((char*)pSrcCur, pSrcMax-pSrcCur);
         if (l > 0) {
            if (pDstCur+l >= pDstMax)
               break;
            memcpy(pDstCur, pSrcCur, l);
            pDstCur += l;
            pSrcCur += l;
            continue;
         }
      }

      uint c = *pSrcCur;
      int  n = toutf8(pDstCur, pDstMax-pDstCur, c);
      if (n < 1)
         break;
      if (pDstCur >= pDstMax)
         break;
      pDstCur += n;
      pSrcCur++;
   }

   if (pDstCur >= pDstMax)
      pDstCur = pDstMax-1;

   *pDstCur = '\0';

   return pDstCur - pszOut;
}

double getcalcval(char *psz, char **pcont)
{
   if (!strncmp(psz, "0x", 2)) {
      // must be unsigned long integer
      #ifdef _MSC_VER
      return (double)_strtoui64(psz+2, pcont, 0x10);
      #else
      return (double)strtoull(psz+2, pcont, 0x10);
      #endif
   } else {
      // accept signed double
      return strtod(psz, pcont);
   }
}

extern cchar *pszGlblBlank;

int sfkcalc(double &r, char *pszFrom, char **ppNext, int iLevel, bool bStopPM)
{
   double v1=0.0,v2=0.0;

   if (cs.debug) printf("%.*s[%s]\n",iLevel*2,pszGlblBlank,pszFrom);

   char *pszNext=0;
   int   iOwnBra=0;

   skipWhite(&pszFrom); // sfk1962

   if (isdigit(*pszFrom)!=0 || *pszFrom=='-') {
      v1=getcalcval(pszFrom,&pszNext);
      pszFrom=pszNext;
   } else if (cs.brackets==1 && *pszFrom=='(') {
      pszFrom++;
      iOwnBra++;
      if (sfkcalc(v1,pszFrom,&pszNext,iLevel+1))
         return 9;
      pszFrom=pszNext;
   }
     else return 9+perr("invalid: %s\n",pszFrom);

   bool bstop=0;
   while (*pszFrom!=0 && bstop==0)
   {
      skipWhite(&pszFrom); // sfk1962
      if (*pszFrom==0)     // sfk1962
         break;

      if (cs.debug) printf("%.*s[%s]\n",iLevel*2,pszGlblBlank,pszFrom);

      // get * / + -
      char c = *pszFrom++;
      switch (c)
      {
         case ')':
            if (!cs.brackets)
               return 9+perr("invalid: %s\n",pszFrom-1);
            if (cs.debug) printf("%.*s) %f\n",iLevel*2,pszGlblBlank,v1);
            if (iOwnBra-- <= 0) {
               bstop=1;
               pszFrom--;
            }
            continue;
         case '*':
            if (cs.brackets==1 && *pszFrom=='(') {
               pszFrom++;
               iOwnBra++;
               if (sfkcalc(v2,pszFrom,&pszNext,iLevel+1))
                  return 9;
            } else {
               v2=getcalcval(pszFrom,&pszNext);
            }
            if (cs.debug) printf("%.*s%f * %f = %f\n",iLevel*2,pszGlblBlank,v1,v2,v1*v2);
            v1 *= v2;
            pszFrom=pszNext;
            continue;
         case '/':
            if (cs.brackets==1 && *pszFrom=='(') {
               pszFrom++;
               iOwnBra++;
               if (sfkcalc(v2,pszFrom,&pszNext,iLevel+1))
                  return 9;
            } else {
               v2=getcalcval(pszFrom,&pszNext);
            }
            if (v2 == 0.0)
               return 9+perr("division by zero: %s\n",pszFrom);
            if (cs.debug) printf("%.*s%f / %f = %f\n",iLevel*2,pszGlblBlank,v1,v2,v1/v2);
            v1 /= v2;
            pszFrom=pszNext;
            continue;
         case '+':
            if (bStopPM) {
               pszFrom--;
               bstop=1;
               continue;
            }
            if (sfkcalc(v2, pszFrom, &pszNext, iLevel+1, 1))
               return 9;
            if (cs.debug) printf("%.*s%f + %f = %f\n",iLevel*2,pszGlblBlank,v1,v2,v1+v2);
            v1 += v2;
            pszFrom=pszNext;
            continue;
         case '-':
            if (bStopPM) {
               pszFrom--;
               bstop=1;
               continue;
            }
            if (sfkcalc(v2, pszFrom, &pszNext, iLevel+1, 1))
               return 9;
            if (cs.debug) printf("%.*s%f - %f = %f\n",iLevel*2,pszGlblBlank,v1,v2,v1-v2);
            v1 -= v2;
            pszFrom=pszNext;
            continue;
         default:
            return 9+perr("invalid: %s\n",pszFrom-1);
      }
   }

   r=v1;
   if (ppNext) *ppNext=pszFrom;

   return 0;
}

bool bGlblGotToMake = 0;

uchar *newBitField(int iTotalEntries)
{
   int iByteSize = (iTotalEntries / 8) + 1;
   int iTolerance= 4;
   uchar *pField = new uchar[iByteSize+iTolerance];
   if (!pField)
      return 0;
   memset(pField, 0, iByteSize+iTolerance);
   int *pSize = (int *)pField;
   *pSize = iTotalEntries;
   return pField;
}
// can be freed by delete [] pField
// sfkSetBit, sfkGetBit see sfkbase.hpp

// - - - internal support functions end

// ====== SFK crash handler begin ========

#ifndef USE_SFK_BASE

struct SFKSysLog
{
   SFKSysLog ( );

   uint aSysLog[256];
   unsigned char iSysLog;
};

char *pszGlblSysLogFileName = 0;

SFKSysLog::SFKSysLog( ) { memset(this, 0, sizeof(*this)); }

struct SFKSysLog glblDefaultSysLog, glblDummySysLog;

struct SFKSysLog *pGlblSysLog = &glblDefaultSysLog;

#define MyModuleId 0 // not yet used

#ifndef MTK_TRACE
 #ifndef USE_SFK_BASE
  #undef __
  #define __                 \
     pGlblSysLog->aSysLog[pGlblSysLog->iSysLog++] = \
        ((__LINE__ & 0xFFFFU) << 16) \
      | (MyModuleId & 0xFFFFU);
 #endif
#endif

static void showSysLog( )
{
   static char szBuf[2048];
   static char szWord[100];
   static const char *ahex = "0123456789ABCDEF";

   struct SFKSysLog *pCurLog = pGlblSysLog;

   // disable further writes to main log
   pGlblSysLog = &glblDummySysLog;

   int istart=0,icur=0,isteps=0;
   uint imix,iline=0,ithread=0;
   char *pdst=0,*pmax=szBuf+sizeof(szBuf)-100;

   istart = pCurLog->iSysLog;
   icur   = istart;
   if (icur > 0) icur--; else icur = 0xFFU;

   pdst = szBuf;

   strcat(pdst, "program version: SFK " SFK_VERSION SFK_FIXPACK "\n"
                "\n=== last steps: ===\n");
   pdst += strlen(pdst);

   for (; isteps < 100 && pdst < pmax; isteps++)
   {
      imix = pCurLog->aSysLog[icur];
      iline = (imix >> 16) & 0xFFFFU;
      ithread = (imix & 0xFFFFU);

      // 01.00000
      // 012345678
      pdst[0+1]  = ahex[(ithread >>  0) & 0x0F];
      pdst[0+0]  = ahex[(ithread >>  4) & 0x0F];
      pdst[0+2]  = '.';
      pdst[3+4]  = ahex[(iline /     1) % 10];
      pdst[3+3]  = ahex[(iline /    10) % 10];
      pdst[3+2]  = ahex[(iline /   100) % 10];
      pdst[3+1]  = ahex[(iline /  1000) % 10];
      pdst[3+0]  = ahex[(iline / 10000) % 10];
      pdst[8]    = ((isteps % 5)==4) ? '\n' : ' ';
      pdst += 9;

      if (icur > 0) icur--; else icur = 0xFFU;
   }

   *pdst = '\0';

   if (pszGlblSysLogFileName)
   {
      strcat(pdst, "=== see also: ");
      strcat(pdst, pszGlblSysLogFileName);
      strcat(pdst, " ===\n");
 
      // try to write a report file
      FILE *fout = fopen(pszGlblSysLogFileName,"w");
      if (fout)
      {
         fwrite(szBuf, 1, strlen(szBuf), fout);
         fclose(fout);
      }
   }
 
   #ifdef _WIN32

   MessageBox(0, szBuf, "fatal error", MB_OK);

   #else

   fprintf(stderr, "=== fatal error during execution ===\n%s\n", szBuf);

   #endif

   exit(-1);
}

static void crashTest( )
{
   char *p = 0;
   *p = 'a';
}

static void traceModeCrashHandler(int sig)
{
   static int crashRecursionCounter = 0;
   crashRecursionCounter++;
   if (crashRecursionCounter < 2)
   {
      crashRecursionCounter++;

      #ifdef WITH_TRACING
      // dump stack trace to error log
      mtkDumpLastSteps(0);
      mtkDumpStackTrace(0);
      #else
      showSysLog();
      #endif
   }
   printf("sfk exits due to segment violation.\n");
   exit(255);
}

void initCrashHandler(char *pszDumpFile)
{
   pszGlblSysLogFileName = pszDumpFile; // if any

   void (*pfhand)(int) = traceModeCrashHandler;
   sigset_t mask;
   sigemptyset(&mask);
#ifdef SIGSEGV
   signal (SIGSEGV, pfhand);
   sigaddset(&mask, SIGSEGV);
#else
   #error no_sigsegv_defined
#endif
#ifdef SIGFPE
   signal (SIGFPE, pfhand);
   sigaddset(&mask, SIGFPE);
#else
   #error no_sigfpe_defined
#endif
#ifdef SIGILL
   signal (SIGILL, pfhand);
   sigaddset(&mask, SIGILL);
#else
   #error no_sigill_defined
#endif
#ifdef SIGABRT
   signal (SIGABRT, pfhand);
   sigaddset(&mask, SIGABRT);
#else
   #error no_sigabrt_defined
#endif
   sigprocmask(SIG_UNBLOCK, &mask, 0);
}

#else

static void crashTest( ) { }

#endif // USE_SFK_BASE

// ====== SFK trace mode crash handler end ========

const char *pGlblHttpUserAgent = 0;

static char szGlblUserAgent[100];

void setHTTPUserAgent(const char *psz)
   { pGlblHttpUserAgent = psz; }

char *getHTTPUserAgent()
{
   if (pGlblHttpUserAgent)
      return str(pGlblHttpUserAgent); // sfk1933

   if (bGlblCurCmdSet)
      snprintf(szGlblUserAgent, sizeof(szGlblUserAgent)-10,
         "Swiss File Knife/" SFK_VERSION " (sfk %s, " VER_STR_OS ")",
            szGlblCurCmd);
   else
      snprintf(szGlblUserAgent, sizeof(szGlblUserAgent)-10,
         "Swiss File Knife/" SFK_VERSION " (" VER_STR_OS ")");

   char *psz = strstr(szGlblUserAgent, "windows-any)");
   if (psz) strcpy(psz, "windows)");


   return szGlblUserAgent;
}

// optional callback tracing for hot spots
void (*pGlblTraceCallback)(char *pmsg) = 0;
static char szTraceBuf[1024+10];
void cbtrace(const char *pszFormat, ...)
{
   if (!pGlblTraceCallback) return;
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szTraceBuf, sizeof(szTraceBuf)-10, pszFormat, argList);
   szTraceBuf[sizeof(szTraceBuf)-10] = '\0';
   pGlblTraceCallback(szTraceBuf);
}


// set color attribs for a string buffer.
// this also sets zero terminator at end of string,
// i.e. nmaxlen means including zero terminator.
void setattr(char *pdst, uchar uc, uint nlen, uint nmaxlen)
{
   if (nlen > nmaxlen) nlen = nmaxlen;
   if (nlen > 0) {
      memset(pdst, uc, nlen-1);
      pdst[nlen-1] = '\0';
   } else {
      pdst[0] = '\0';
   }
}

#ifdef WINFULL

// windows virtual wide char support

#define wstrlen  wcslen    // use mystrwlen
#define wstrchr  wcschr
#define wstrrchr wcsrchr

sfkname::sfkname(const char *psz, bool bpure)
{
   memset(this, 0, sizeof(*this));
   strcopy(szname, psz);
   nstate = 1;
}

sfkname::sfkname(ushort *pwsz)
{
   memset(this, 0, sizeof(*this));
   wcsncpy((wchar_t*)awname, (wchar_t*)pwsz, sfkmaxname);
   nstate = 2;
}

ushort *sfkname::wname( )
{
   bbadconv = 0;

   if (nstate & 2)
      return awname;
   if (!(nstate & 1))
      return 0;

   awname[0] = 0;

   // convert from szname
   #ifdef SFK_UNAME
   if (cs.uname)
   {
      mclear(awname);
      int irc = MultiByteToWideChar(   // win.wname
         CP_UTF8, 0,
         szname, strlen(szname),
         (wchar_t*)awname, sfkmaxname
         );
      if (!irc) bbadconv = 1;
   }
   else if (cs.aname)
   {
      mclear(awname);
      int irc = MultiByteToWideChar(   // win.aname
         CP_ACP, 0,
         szname, strlen(szname),
         (wchar_t*)awname, sfkmaxname
         );
      if (!irc) bbadconv = 1;
   }
   #endif // SFK_UNAME

   if (cs.tname) // tname.decode
   {
      ushort *pdstcur = awname;
      ushort *pdstmax = pdstcur+sfkmaxname;
      int i=0;
      for (; pdstcur<pdstmax && szname[i]!=0; i++)
      {
         if (   szname[i+0]=='<'
             && isxdigit(szname[i+1])
             && isxdigit(szname[i+2])
             && isxdigit(szname[i+3])
             && isxdigit(szname[i+4])
             && szname[i+5]=='>'
            )
         {
            ushort c = (ushort)strtoul(szname+i+1,0,0x10);
            *pdstcur++ = c;
            i += 5;
            continue;
         }
         *pdstcur++ = ((ushort)szname[i]) & 0xFFU;
      }
      *pdstcur = 0;
   }

   return awname;
}

char *sfkname::vname( )
{
   bbadconv = 0;

   if (nstate & 1)
      return szname;
   if (!(nstate & 2))
      return 0;

   szname[0] = '\0';

   // convert from wname
   #ifdef SFK_UNAME
   if (cs.uname)
   {
      // result is NOT zero terminated!
      mclear(szname);
      int irc = WideCharToMultiByte(   // vname.utf8
         CP_UTF8, 0,
         (wchar_t*)awname, wcslen((wchar_t*)awname),
         szname, sfkmaxname, 0, 0);
      if (!irc) bbadconv = 1;
   }
   else if (cs.aname)
   {
      int irc = sfkchars.strunitoansi(awname, sfkchars.wlen(awname),
         szname, sfkmaxname);
      if (irc > 0) bbadconv = 1;
   }
   #endif // SFK_UNAME

   if (cs.tname) // tname.encode
   {
      char *pdstcur = szname;
      char *pdstmax = szname+sfkmaxname;
      int i=0;
      for (; pdstcur<pdstmax && awname[i]!=0; i++)
      {
         ushort c = awname[i];
         if (c < 0x0100U) {
            *pdstcur++ = (char)c;
            continue;
         }
         sprintf(pdstcur, "<%04X>", c);
         pdstcur += 6;
      }
      *pdstcur = '\0';
   }

   return szname;
}

#endif // WINFULL

// for windows: convert "/" to "\\"
void setSystemSlashes(char *pdst)
{
   #ifdef _WIN32
   for (; *pdst; pdst++)
      if (*pdst == glblWrongPChar)
          *pdst = glblPathChar;
   #endif
}

void setNetSlashes(char *pdst)
{
   for (; *pdst; pdst++)
      if (*pdst == '\\')
          *pdst = '/';
}

#ifdef _WIN32
int mygetpos(FILE *f, num &rpos, char *pszFile)
{
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   rpos = (num)npos1;
   return 0;
}
int mysetpos(FILE *f, num pos, char *pszFile)
{
   fpos_t npos1 = (fpos_t)pos;
   if (fsetpos(f, &npos1))
      return 9+perr("setpos failed on %s\n", pszFile);
   return 0;
}
#else
int mygetpos(FILE *f, num &rpos, char *pszFile)
{
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   #if defined(MAC_OS_X) || defined(SOLARIS)
   rpos = (num)npos1;
   #else
   rpos = (num)npos1.__pos;
   #endif
   return 0;
}
int mysetpos(FILE *f, num pos, char *pszFile)
{
   // fetch "status" first
   fpos_t npos1;
   if (fgetpos(f, &npos1))
      return 9+perr("getpos failed on %s\n", pszFile);
   #if defined(MAC_OS_X) || defined(SOLARIS)
   npos1 = (fpos_t)pos;
   #else
   npos1.__pos = (__off_t)pos;
   #endif
   if (fsetpos(f, &npos1))
      return 9+perr("setpos failed on %s\n", pszFile);
   return 0;
}
#endif

// simple check: if a file contains some nulls, it must be binary.
bool isBinaryFile(char *pszFile)
{
   // using same probe size as Coi::isBinaryFile
   int nCheckLen = MY_GETBUF_MAX - 10;

   FILE *fin = fopen(pszFile, "rb");
   if (!fin) return 0;

   if (nCheckLen > (int)sizeof(abBuf)-10)
       nCheckLen = (int)sizeof(abBuf)-10;

   int nRead = fread(abBuf, 1, nCheckLen, fin);
   fclose(fin);

   // for (int i=0; i<nRead; i++)
   //    if (abBuf[i] == 0x00)
   //       return 1;
   if (memchr(abBuf, 0, nRead))
      return 1;

   return 0;
}

bool isWhitespace(char *psz, int nlen)
{
   for (int i=0; psz[i] && (i<nlen); i++)
      if (!isspace(psz[i]))
         return 0;
   return 1;
}

bool isEmpty(char *psz)
{
   for (; *psz; psz++)
      if (!isspace(*psz))
         return 0;
   return 1;
}

bool isDigitOrNull(char c) {
   if (c == 0) return 1;
   return isdigit(c);
}

bool ispathchr(char c) {
   if (c=='\\') return 1;
   if (c=='/') return 1;
   return 0;
}

const char *pGlblBlankBuf160 =
   "                                        "
   "                                        "
   "                                        "
   "                                        ";

#ifdef SFK_CCDIRTIME
StringTable glblCreatedDirs;
#endif

bool isNetDriveRoot(char *psz)
{
   #ifdef _WIN32
   if (strncmp(psz, "\\\\", 2)) return 0;
   #else
   if (strncmp(psz, "//", 2)) return 0;
   #endif
   // jump past \\, check if further dir is appended
   char *psz2 = strchr(psz+2, glblPathChar);
   if (!psz2) return 1;   // yes: e.g. \\foomachine
   // is it JUST a slash or something more?
   psz2++;
   if (!strlen(psz2)) return 1; // just the slash
   return 0; // no: e.g. \\foomachine\c$
}

int sfkmkdir(char *pszName, bool braw)
{
   int irc=0;

   #ifdef _WIN32
   if (braw==0 && vname()!=0) {
      sfkname oname(pszName);
      irc = _wmkdir((const wchar_t *)oname.wname());
   } else {
      irc = _mkdir(pszName);
   }
   #else
   irc = mkdir(pszName, S_IREAD | S_IWRITE | S_IEXEC);
   #endif

   return irc;
}

// uses szLineBuf, abBuf
int createSubDirTree(char *pszDstRoot, char *pszDirTree, char *pszRefRoot)
{
   // create all needed target directories
   joinPath(szLineBuf, MAX_LINE_LEN, pszDstRoot, pszDirTree);

   char *psz1 = szLineBuf;
   char *psz2 = 0;
   if (strlen(psz1))
      psz2 = strchr(psz1+1, glblPathChar);
   while (psz2)
   {
      strncpy((char*)abBuf, psz1, psz2-psz1);
      abBuf[psz2-psz1] = 0;
      char *pszDir = (char*)abBuf;

      #ifdef _WIN32
      if (strlen(pszDir)==2 && pszDir[1]==':')
      { } // don't create "c:"
      else
      #endif
      if (isNetDriveRoot(pszDir))
      { }
      else
      if (!isDir(pszDir))
      {
         if (cs.verbose)
            printf("mkdir.1: %s\n", pszDir);
         #ifdef _WIN32
         if (_mkdir(pszDir))
         #else
         if (mkdir(pszDir, S_IREAD | S_IWRITE | S_IEXEC))
         #endif
         {
            perr("cannot create dir: %s\n", pszDir);
            return 9;
         }
         // if ref root is given, copy its timestamp
         // doesn't work here: file created afterwards updates the dir timestamp
         // trace created dir in global list
         #ifdef SFK_CCDIRTIME
         glblCreatedDirs.addEntry(pszDir);
         #endif
         // count created dir
         cs.dirs++;
      }
      psz2 = strchr(psz2+1, glblPathChar);
   }

   char *pszDir = szLineBuf;

   #ifdef _WIN32
   if (strlen(pszDir)==2 && pszDir[1]==':')
   { } // don't create "c:"
   else
   #endif
   if (isNetDriveRoot(pszDir))
   { }
   else
   if (!isDir(pszDir))
   {
      if (cs.verbose)
         printf("mkdir.2: %s\n", pszDir);
      #ifdef _WIN32
      if (_mkdir(pszDir))
      #else
      if (mkdir(pszDir, S_IREAD | S_IWRITE | S_IEXEC))
      #endif
      {
         perr("cannot create dir: %s\n", pszDir);
         return 9;
      }
      #ifdef SFK_CCDIRTIME
      glblCreatedDirs.addEntry(pszDir);
      #endif
      cs.dirs++;
   }
   return 0;
}

// create dir tree required to write the specified filename.
// uses: szLineBuf, abBuf
// note: does NOT add to glblCreatedDirs
//       does not count created dirs.
int createOutDirTree(char *pszOutFile, KeyMap *pOptMap, bool bForDir)
{
   if (cs.debug)
      printf("createOutDirTree for: %s\n", pszOutFile);

   // isolate path from filename
   // c:\foo\bar.txt -> c:\foo
   // c:foo\bar.txt -> c:\foo
   // c:bar.txt -> skip
   strcopy(szLineBuf, pszOutFile);

   if (!bForDir) {
      char *psz = strrchr(szLineBuf, glblPathChar);
      if (!psz) return 0; // nothing to do
      *psz = '\0';
   }

   char *psz1 = szLineBuf;
   char *psz2 = 0;
   if (strlen(psz1))
      psz2 = strchr(psz1+1, glblPathChar);
   while (psz2)
   {
      strncpy((char*)abBuf, psz1, psz2-psz1);
      abBuf[psz2-psz1] = 0;
      char *pszDir = (char*)abBuf;
      #ifdef _WIN32
      if (strlen(pszDir)==2 && pszDir[1]==':')
      { } // don't create "c:"
      else
      #endif
      if (isNetDriveRoot(pszDir))
      { }
      else
      if (!isDir(pszDir))
      {
         if (cs.verbose)
            printf("mkdir.3: %s\n", pszDir);
         #ifdef _WIN32
         if (_mkdir(pszDir))
         #else
         if (mkdir(pszDir, S_IREAD | S_IWRITE | S_IEXEC))
         #endif
         {
            perr("cannot create dir: %s\n", pszDir);
            return 9;
         }
         if (pOptMap != 0)
            pOptMap->put(pszDir, 0);
      }
      psz2 = strchr(psz2+1, glblPathChar);
   }

   char *pszDir = szLineBuf;

   #ifdef _WIN32
   if (strlen(pszDir)==2 && pszDir[1]==':')
   { } // don't create "c:"
   else
   #endif
   if (isNetDriveRoot(pszDir))
   { }
   else
   if (!isDir(pszDir))
   {
      if (cs.verbose)
      printf("mkdir.4: %s\n", pszDir);
      #ifdef _WIN32
      if (_mkdir(pszDir))
      #else
      if (mkdir(pszDir, S_IREAD | S_IWRITE | S_IEXEC))
      #endif
      {
         perr("cannot create dir: %s\n", pszDir);
         return 9;
      }
      if (pOptMap != 0)
         pOptMap->put(pszDir, 0);
   }
   return 0;
}

#ifdef WINFULL
bool isNetDriveRootW(ushort *psz)
{
   if (psz[0]!='\\' || psz[1]!='\\') return 0;

   wchar_t *psz2 = wstrchr((wchar_t*)psz+2, (wchar_t)glblPathChar);
   if (!psz2) return 1;

   psz2++;
   if (!wstrlen((wchar_t*)psz2)) return 1;

   return 0;
}
int isDirW(ushort *pszName)
{
   DWORD nAttrib = GetFileAttributesW((wchar_t*)pszName);
   if (nAttrib == 0xFFFFFFFF) // "INVALID_FILE_ATTRIBUTES"
      return 0;
   if (nAttrib & FILE_ATTRIBUTE_DIRECTORY)
      return 1;
   return 0;
}
int createOutDirTreeW(char *pszOutFile, KeyMap *pOptMap, bool bForDir)
{
   strcopy(szLineBuf, pszOutFile);

   if (!bForDir) {
      char *psz = strrchr(szLineBuf, glblPathChar);
      if (!psz) return 0; // nothing to do
      *psz = '\0';
   }

   char *psz1 = szLineBuf;
   char *psz2 = 0;
   if (strlen(psz1))
      psz2 = strchr(psz1+1, glblPathChar);
   while (psz2)
   {
      strncpy((char*)abBuf, psz1, psz2-psz1);
      abBuf[psz2-psz1] = 0;
      char *pszDirA = (char*)abBuf;

      sfkname oname(pszDirA);
      ushort *pDir = oname.wname();

      if (wstrlen((wchar_t*)pDir)==2 && pDir[1]==':')
      { } // don't create "c:"
      else
      if (isNetDriveRootW(pDir))
      { }
      else
      if (!isDirW(pDir))
      {
         if (_wmkdir((wchar_t*)pDir))
         {
            perr("cannot create dir: %s\n", pszDirA);
            pinf("... trying wmkdir: %s\n", dataAsTrace(pDir,wcslen((wchar_t*)pDir)*2));
            return 9;
         }
         if (pOptMap != 0)
            pOptMap->put(pszDirA, 0);
      }
      psz2 = strchr(psz2+1, glblPathChar);
   }

   char *pszDirA = szLineBuf;

   sfkname oname(pszDirA);
   ushort *pDir = oname.wname();

   if (wstrlen((wchar_t*)pDir)==2 && pDir[1]==':')
   { } // don't create "c:"
   else
   if (isNetDriveRootW(pDir))
   { }
   else
   if (!isDirW(pDir))
   {
      if (_wmkdir((wchar_t*)pDir))
      {
         perr("cannot create dir: %s\n", pszDirA);
         pinf("... trying wmkdir: %s\n", dataAsTrace(pDir,wcslen((wchar_t*)pDir)*2));
         return 9;
      }
      if (pOptMap != 0)
         pOptMap->put(pszDirA, 0);
   }
   return 0;
}

#endif

int hexToBin(char *pszHex, uchar *pBin, uint nBinLen)
{
   int nBinRem = (int)nBinLen; // remaining out buffer
   char szBuf[10];
   memset(szBuf, 0, sizeof(szBuf));
   while (*pszHex && (nBinRem > 0)) {
      szBuf[0] = *pszHex++;
      if (!*pszHex) return 1;
      szBuf[1] = *pszHex++;
      if (!isxdigit(szBuf[0])) return 2;
      if (!isxdigit(szBuf[1])) return 2;
      *pBin++ = (uchar)strtol(szBuf, 0, 0x10);
      nBinRem--;
   }
   if (*pszHex || nBinRem)
      return 3;
   return 0;
}

uchar *memFind(uchar *pNeedle, num nNeedleSize, uchar *pHayStack, num nHaySize)
{
   uchar *pCur = pHayStack;
   uchar *pMax = pHayStack + nHaySize - nNeedleSize; // inclusive
   uchar c1    = *pNeedle;
   while (pCur <= pMax)
   {
      uchar *p1 = pCur;
      // seek to next potential start
      // while ((p1 <= pMax) && (*p1 != c1))
      //   p1++;
      p1 = (uchar*)memchr(p1, c1, pMax - p1 + 1); // +1: pMax is inclusive
      if (!p1 || (p1 > pMax))
         return 0;
      // compare
      if ((p1 <= pMax) && !memcmp(p1, pNeedle, nNeedleSize))
         return p1;  // hit
      // no hit, proceed
      pCur = p1+1;
   }
   return 0;
}

// - - - option -spat slash pattern support

void getUpto(int iMaxCopy, char *pszSrc, int iMaxSrc, char *pszBuf, int iMaxBuf)
{
   int i=0;
   for (i=0; (i<iMaxCopy) && (i<iMaxBuf-1); i++)
   {
      if (iMaxSrc > 0) {
         pszBuf[i] = pszSrc[i];
         iMaxSrc--;
      } else {
         pszBuf[i] = ' ';
      }
   }
   pszBuf[i] = '\0';
}

int shrinkFormTextVar(char *psz, int &rLen, uchar **ppFlags)
{
   if (!cs.spat)
      return 0; // nothing to do

   bool bstrict = (cs.spat >= 2) ? 1 : 0;

   return shrinkFormTextBlock(psz, rLen, bstrict, cs.xchars, ppFlags);
}

// replace \\t and \\xnn by a single character,
// within a binary block that is not zero-terminated.
int shrinkFormTextBlock(char *psz, int &rLen, bool bstrict, bool xchars, uchar **ppFlags)
{
   char szBuf[20];

   int iCodePoints = 0;
 
   uchar uc = 0;
   bool bAnyX = 0;
   uchar *pFlags = 0;
   uchar *pDst = 0;

   char *pszMax = psz + rLen;

   for (int ipass=0; ipass<2; ipass++)
   {
      char *pszCur = psz;

      if (ipass)
      {
         if (!xchars && bAnyX && ppFlags)
         {
            if (!(pFlags = newBitField(iCodePoints+4)))
               return 9+perr("out of memory");
            *ppFlags = pFlags;
         }

         pDst = (uchar*)psz;
      }

      iCodePoints = 0;

      while (pszCur < pszMax)
      {
         uc = *pszCur++;

         if (uc == '\\' && pszCur < pszMax)  // spat.1 shrinktext 2
         {
            switch (*pszCur++)
            {
               case '\\':  uc = '\\';  break;
               case 't' :  uc = '\t';  break;
               case 'q' :  uc = '"';   break;
               case 'n' :  uc = '\n';  break;
               case 'r' :  uc = '\r';  break;
               case ' ' :  uc = ' ';   break;
               case 'x' :
               {
                  if (pszCur+2 > pszMax) {
                     getUpto(4, pszCur-2, 4, szBuf, sizeof(szBuf)-4);
                     perr("incomplete hex slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                     return 9;
                  }
                  int n = getTwoDigitHex(pszCur);
                  if (n < 0) {
                     getUpto(4, pszCur-2, 4, szBuf, sizeof(szBuf)-4);
                     perr("wrong hex slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                     return 9;
                  }
                  pszCur += 2;
                  uc = (uchar)n;
                  if (pFlags)
                     sfkSetBit(pFlags, iCodePoints);
                  bAnyX = 1;
                  break;
               }
               case 'd' : // sfk 1723 added: "\d255"
               if (bstrict)
               {
                  if (pszCur+3 > pszMax) {
                     getUpto(5, pszCur-2, 5, szBuf, sizeof(szBuf)-4);
                     perr("incomplete decimal slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                     return 9;
                  }
                  int n = getThreeDigitDec(pszCur);
                  if (n < 0 || n > 255) {
                     getUpto(5, pszCur-2, 5, szBuf, sizeof(szBuf)-4);
                     perr("wrong decimal slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                     return 9;
                  }
                  pszCur += 3;
                  uc = (uchar)n;
                  if (pFlags)
                     sfkSetBit(pFlags, iCodePoints);
                  bAnyX = 1;
                  break;
               }
               default:
                  if (bstrict) {
                     getUpto(2, pszCur-1, 2, szBuf, sizeof(szBuf)-4);
                     perr("undefined slash pattern: %.2s   (use \\%.2s instead)\n", szBuf, szBuf);
                     return 9;
                  }
            }  // endswitch
         }  // endif

         if (ipass && pDst)
            *pDst++ = uc;

         iCodePoints++;
      }
   }

   if (pDst < (uchar*)pszMax)
      *pDst = '\0';

   rLen = (int)(pDst - (uchar*)psz);

   return 0;
}

int copyFromFormText(char *pSrc, int iMaxSrc, char *pDstIn, int iMaxDst, uint nflags)
{
   char szBuf[20];

   char *pszCur = pSrc;
   char *pszMax = pSrc+iMaxSrc;

   char *pDst   = pDstIn;
   char *pDstMax= pDstIn+iMaxDst;

   if (iMaxDst > 0)
      pDstIn[0] = '\0'; // sfk1972 safety

   uchar uc;

   while (pszCur < pszMax && pDst+10 < pDstMax)
   {
      uc = *pszCur++;

      if (uc == '\\' && pszCur < pszMax)  // spat.2 copytext 2
      {
         // optionally escape phraser sequences
         char c = *pszCur;
         if ((nflags & 2) && (c==',' || c=='+' || c=='\t'))
            { }
         else
         switch (*pszCur++)
         {
            case '\\':  uc = '\\';  break;
            case 't' :  uc = '\t';  break;
            case 'q' :  uc = '"';   break;
            case 'n' :  uc = '\n';  break;
            case 'r' :  uc = '\r';  break;
            case ' ' :  uc = ' ';   break;
            case 'x' :
            {
               if (pszCur+2 > pszMax) {
                  getUpto(4, pszCur-2, 4, szBuf, sizeof(szBuf)-4);
                  perr("incomplete hex slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                  return -1;
               }
               int n = getTwoDigitHex(pszCur);
               if (n < 0) {
                  getUpto(4, pszCur-2, 4, szBuf, sizeof(szBuf)-4);
                  perr("wrong hex slash pattern: %.4s   (use \\%.4s instead?)\n", szBuf, szBuf);
                  return -1;
               }
               pszCur += 2;
               uc = (uchar)n;
               break;
            }
            case 'd' :
            {
               if (pszCur+3 > pszMax) {
                  getUpto(5, pszCur-2, 5, szBuf, sizeof(szBuf)-4);
                  perr("incomplete dec slash pattern: %.5s   (use \\%.5s instead?)\n", szBuf, szBuf);
                  return -1;
               }
               int n = getThreeDigitDec(pszCur);
               if (n < 0) {
                  getUpto(5, pszCur-2, 5, szBuf, sizeof(szBuf)-4);
                  perr("wrong dec slash pattern: %.5s   (use \\%.5s instead?)\n", szBuf, szBuf);
                  return -1;
               }
               pszCur += 3;
               uc = (uchar)n;
               break;
            }
            default:
               getUpto(2, pszCur-1, 2, szBuf, sizeof(szBuf)-4);
               perr("undefined slash pattern: %.2s   (use \\\\%.2s instead?)\n", szBuf, szBuf);
               return -1;
         }
      }

      *pDst++ = uc;
   }

   *pDst = '\0';

   return (int)(pDst-pDstIn);
}

// - - - sfk network functions



// set/provide a minimum info about the current I/O operation.
// if called multithreaded, this should not crash but in the
// worst case return only incomplete info strings.

class IOStatus {
public:
      IOStatus  ( );

      void   setInfo (cchar *pinfo);
      char  *getInfo (num &nagemsec, num &nbytes, num &nmaxbytes);

      num   countBytes  (num nbytes);
      void  setMaxBytes (num nbytes);
      void  resetBytes  ( );

private:
      char  szClStatus[200+20];
      num   nClSince;
      num   nClBytes;
      num   nClMaxBytes;
};
static IOStatus iostat;

IOStatus::IOStatus() {
   memset(this, 0, sizeof(*this));
}

void IOStatus::setInfo(cchar *pinfo) {
   memset(szClStatus, 0, sizeof(szClStatus));
   if (!pinfo) {
      nClSince = 0;
      return;
   }
   strncpy(szClStatus, pinfo, sizeof(szClStatus)-20);
   nClSince = getCurrentTime();
}

char *IOStatus::getInfo(num &nagemsec, num &nbytes, num &nmaxbytes) {
   if (!szClStatus[0]) return 0;
   if (nClSince > 0) {
      num nage  = getCurrentTime() - nClSince;
      nagemsec  = nage;
      nbytes    = nClBytes;
      nmaxbytes = nClMaxBytes;
   } else {
      nagemsec  = 0;
      nbytes    = 0;
      nmaxbytes = 0;
   }
   return szClStatus;
}

num IOStatus::countBytes(num nbytes) {
   nClBytes += nbytes;
   return nClBytes;
}

void IOStatus::setMaxBytes(num nbytes) {
   nClMaxBytes = nbytes;
}

void IOStatus::resetBytes() {
   nClBytes = 0;
   nClMaxBytes = 0;
}

IOStatusPhase::IOStatusPhase(cchar *pinfo) {
   iostat.setInfo(pinfo);
}

IOStatusPhase::~IOStatusPhase( ) {
   // iostat.setInfo(0);
}

char *getIOStatus(num &nage, num &nbytes, num &nmaxbytes) {
   return iostat.getInfo(nage, nbytes, nmaxbytes);
}
num   countIOBytes(num nbytes)   { return iostat.countBytes(nbytes); }
void  setIOStatMaxBytes(num n)   { iostat.setMaxBytes(n); }
void  resetIOBytes( )   { iostat.resetBytes(); }
void  resetIOStatus( )  { iostat.setInfo(0); iostat.resetBytes(); }

// - - - command chaining support



bool bGlblAllowCtrlCExit = 1;

class CtrlCCover {
public:
      CtrlCCover  (bool bAllowExit);
     ~CtrlCCover  ( );
};

CtrlCCover::CtrlCCover(bool b)
   { bGlblAllowCtrlCExit = b; }
 
CtrlCCover::~CtrlCCover( )
   { bGlblAllowCtrlCExit = 1; }

#define DisableCtrlCProcessExit() CtrlCCover oCtrlCCover(0)

#ifdef _WIN32
// need this to ensure that commands dumping colored output
// do never leave the shell in a non-std color.
BOOL WINAPI ctrlcHandler(DWORD type)
{
   if (type != CTRL_C_EVENT && type != CTRL_BREAK_EVENT)
      return 0;

   bGlblEscape = 1;
 
   setTextColor(-1);

   if (bGlblAllowCtrlCExit)
   {
      // do this only with exit allowed,
      // otherwise main program may crash
      // as it continues on the file handle.
      cleanupFileWrite();
      ExitProcess(8);
   }

   return 1;
}
#else
// unix todo: ctrl+c handler to reset color
void ctrlcHandler(int sig_number)
{
   bGlblEscape = 1;
 
   setTextColor(-1, 0); // stdout
   setTextColor(-1, 1); // stderr

   if (bGlblAllowCtrlCExit)
   {
      // do this only with exit allowed,
      // otherwise main program may crash
      // as it continues on the file handle.
      cleanupFileWrite();
      exit(8);
   }
}
#endif

// - - - output color and console control

#ifdef _WIN32

#define CCMASK_FOREGROUND (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY)
#define CCMASK_BACKGROUND (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY)
#define CCMASK_FORE_BACK  (CCMASK_FOREGROUND|CCMASK_BACKGROUND)
#define CCMASK_NOT_FGCOL  (0xFFFF ^ CCMASK_FOREGROUND)

#endif

void setColorScheme(cchar *psz1)
{
   char *psz2 = 0;
   bool bany = 0;

   if (!strncmp(psz1, "off", 3)) {
      gs.usecolor = gs.usehelpcolor = 0;
      cs.usecolor = cs.usehelpcolor = 0;
   }
   else
   if (!strncmp(psz1, "on", 2)) {
      gs.usecolor = 1;
      cs.usecolor = 1;
   }

   if (strstr(psz1, "bright"))
      { nGlblDarkColBase = 1; bany=1; }

   if (strstr(psz1, "dark"))
      { nGlblBrightColBase = 0; bany=1; }

   #ifdef MAC_OS_X // 1694
   if (strstr(psz1, "theme:black") || !strcmp(psz1, "black") || !strcmp(psz1, "b")) {
      nGlblDarkColBase    =  0;
      nGlblBrightColBase  =  0;
      nGlblHeadColor      =  4;
      nGlblExampColor     =  6;
      nGlblFileColor      = 10;
      nGlblHitColor       =  4;
      nGlblRepColor       =  6;
      nGlblLinkColor      = 12;
      nGlblWarnColor      =  6;
      nGlblDefColor       = 14;
      bany=1;
   }
   if (strstr(psz1, "theme:white") || !strcmp(psz1, "white") || !strcmp(psz1, "w")) {
      nGlblDarkColBase    =  0;
      nGlblBrightColBase  =  0;
      nGlblHeadColor      =  4;
      nGlblExampColor     = 10;
      nGlblFileColor      = 10;
      nGlblLinkColor      = 12;
      nGlblHitColor       =  4;
      nGlblRepColor       =  6;
      nGlblErrColor       =  2;
      nGlblWarnColor      = 10;
      nGlblPreColor       =  8;
      nGlblTimeColor      =  8;
      nGlblTraceIncColor  = 12;
      nGlblTraceExcColor  = 10;
      nGlblDefColor       =  0;
      bany=1;
   }
   #else
   if (strstr(psz1, "theme:black") || !strcmp(psz1, "black") || !strcmp(psz1, "b")) {
      // bright colors with black background
      nGlblDarkColBase =  1;
      nGlblExampColor  =  7; // bright yellow
      nGlblHeadColor   =  5; // green
      nGlblFileColor   =  5; // green
      nGlblHitColor    = 11; // purple
      nGlblRepColor    =  7; // bright yellow
      nGlblLinkColor   = 13; // bright cyan
      nGlblWarnColor   =  7; // bright yellow
      #ifndef _WIN32
      // nGlblDefColor = 14; // fix: 1732: don't use gray but terminal default
      #endif
      bany=1;
   }
   if (strstr(psz1, "theme:white") || !strcmp(psz1, "white") || !strcmp(psz1, "w")) {
      // with white background, user may want to select
      // bright or dark colors case by case.
      nGlblHeadColor      =  4; // green
      nGlblExampColor     = 11; // purple
      nGlblFileColor      =  4; // green
      nGlblHitColor       = 11; // purple
      nGlblRepColor       =  3; // red
      nGlblLinkColor      = 12; // cyan
      nGlblErrColor       =  3; // red
      nGlblWarnColor      = 11; // purple
      nGlblPreColor       =  8; // blue
      nGlblTimeColor      =  8; // blue
      nGlblTraceIncColor  = 12; // dark magenta
      nGlblTraceExcColor  = 11; // purple
      bany=1;
   }
   #endif

   if (strstr(psz1, ",nobold")) // 1694
      { nGlblDarkColBase = 0; nGlblBrightColBase = 0; bany=1; }
   else
   if (strstr(psz1, ",bold"))   // 1694
      { nGlblDarkColBase = 1; nGlblBrightColBase = 1; bany=1; }

   psz2 = (char*)strstr(psz1, "head:");  if (psz2) { nGlblHeadColor  = atol(psz2+5); bany=1; }
   psz2 = (char*)strstr(psz1, "examp:"); if (psz2) { nGlblExampColor = atol(psz2+6); bany=1; }
   psz2 = (char*)strstr(psz1, "file:");  if (psz2) { nGlblFileColor  = atol(psz2+5); bany=1; }
   psz2 = (char*)strstr(psz1, "link:");  if (psz2) { nGlblLinkColor  = atol(psz2+5); bany=1; }
   psz2 = (char*)strstr(psz1, "hit:");   if (psz2) { nGlblHitColor   = atol(psz2+4); bany=1; }
   psz2 = (char*)strstr(psz1, "rep:");   if (psz2) { nGlblRepColor   = atol(psz2+4); bany=1; }
   psz2 = (char*)strstr(psz1, "err:");   if (psz2) { nGlblErrColor   = atol(psz2+4); bany=1; }
   psz2 = (char*)strstr(psz1, "warn:");  if (psz2) { nGlblWarnColor  = atol(psz2+5); bany=1; }
   psz2 = (char*)strstr(psz1, "pre:");   if (psz2) { nGlblPreColor   = atol(psz2+4); bany=1; }
   #ifndef _WIN32
   psz2 = (char*)strstr(psz1, "def:");   if (psz2) { nGlblDefColor   = atol(psz2+4); bany=1; }
   #endif
   //                   0123456789
   psz2 = (char*)strstr(psz1, "time:");     if (psz2) { nGlblTimeColor     = atol(psz2+5); bany=1; }
   psz2 = (char*)strstr(psz1, "traceinc:"); if (psz2) { nGlblTraceIncColor = atol(psz2+9); bany=1; }
   psz2 = (char*)strstr(psz1, "traceexc:"); if (psz2) { nGlblTraceExcColor = atol(psz2+9); bany=1; }
 
   if (bany) {
      gs.usecolor = 1;
      cs.usecolor = 1;
   }
}

int nGlblCurColor = -1; // currently active color

void setTextColor(int nIn, bool bStdErr, bool bVerbose)
{
   int n = nIn;

   if (n == nGlblCurColor) {
      mtklog(("color: ignore %d, is set already", n));
      return;
   }
   nGlblCurColor = n;

   if (bGlblHtml)
   {
      static bool bAnySet = 0;
      static bool bIsBold = 0;
      uint ncol = 0;
      if (bAnySet) { printf("</font>"); bAnySet=0; }
      if (bIsBold) { printf("</b>"); bIsBold=0; }
      if (n == -1)
         return;
      bAnySet=1;
      if (n == 1) { bIsBold=1; printf("<b>"); }
      if (n &  2) ncol |= (n&1) ? 0xFF0000 : 0x990000;
      if (n &  4) ncol |= (n&1) ? 0x00FF00 : 0x009900;
      if (n &  8) ncol |= (n&1) ? 0x0000FF : 0x000099;
      if (n & 16) { bIsBold=1; printf("<b>"); }
      printf("<font color=\"%06X\">", ncol);
      return;
   }

   if (!cs.usecolor) {
      mtklog(("color: ignore %d, no colors used", n));
      return;
   }

   #ifdef _WIN32

   #ifdef WINFULL
   if (n == 0) {
      // FIX: n==0 produces invisible text both with a white
      //      or black background shell. a visible compromise:
      n = 14;
   }

   WORD nAttrib = 0;
   if (n & 1) nAttrib |= FOREGROUND_INTENSITY;
   if (n & 2) nAttrib |= FOREGROUND_RED;
   if (n & 4) nAttrib |= FOREGROUND_GREEN;
   if (n & 8) nAttrib |= FOREGROUND_BLUE;

   if (n == -1) {
      // default color: set all attribs as they were.
      SetConsoleTextAttribute(hGlblConsole, nGlblConsAttrib);
      mtklog(("color: scta %lxh (is default)", nGlblConsAttrib));
   } else {
      // set new FOREGROUND text color, but make sure that
      // background color and anything else stays unchanged.
      uint nval = (nGlblConsAttrib & CCMASK_NOT_FGCOL) | nAttrib;
      SetConsoleTextAttribute(hGlblConsole, nval);
      mtklog(("color: scta %lxh (default=%lxh)", nval, nGlblConsAttrib));
   }
   #endif

   #else

   #define UXATTR_RESET     0
   #define UXATTR_BRIGHT    1
   #define UXATTR_DIM       2
   #define UXATTR_UNDERLINE 3
   #define UXATTR_BLINK     4
   #define UXATTR_REVERSE   7
   #define UXATTR_HIDDEN    8

   #define UXCOL_BLACK      0
   #define UXCOL_RED        1
   #define UXCOL_GREEN      2
   #define UXCOL_YELLOW     3
   #define UXCOL_BLUE       4
   #define UXCOL_MAGENTA    5
   #define UXCOL_CYAN       6
   #define UXCOL_WHITE      7

   FILE *fout = stdout;
   if (bStdErr)
         fout = stderr;

   if (n==-1) {
      // unix: have to use a user-defined default color
      n = nGlblDefColor;
   }

   int nAttr = (n & 1) ? UXATTR_BRIGHT : UXATTR_RESET;

   switch (n & 14) {
      case  2: n = UXCOL_RED;     break;
      case  4: n = UXCOL_GREEN;   break;
      case  6: n = UXCOL_YELLOW;  break;
      case  8: n = UXCOL_BLUE;    break;
      case 10: n = UXCOL_MAGENTA; break;
      case 12: n = UXCOL_CYAN;    break;
      case 14: n = UXCOL_WHITE;   break;
      default: n = UXCOL_BLACK;   break;
   }

   if (nIn == -1 && nGlblDefColor == 0) {
      // fix: 1732: linux default color if def:0
      if (bVerbose)
         fprintf(fout, "using color sequence: \\x1b[0;0m\n");
      fprintf(fout, "%c[0;0m", 0x1B);
   } else {
      if (bVerbose)
         fprintf(fout, "using color sequence: \\x1b[%d;%dm\n", nAttr, n+30);
      fprintf(fout, "%c[%d;%dm", 0x1B, nAttr, n+30);
   }

   #endif
}

// - - - colored terminal output

// printf with OEM char conversion
void oprintf(cchar *pszFormat, ...);
void oprintf(StringPipe *pOutData, cchar *pszFormat, ...);

int sfkMapAttrToColor(char cAttr)
{
   int d = nGlblDarkColBase;
   int b = nGlblBrightColBase;

   switch (cAttr)
   {
      // sfk internal color codes. when extending this,
      // also adapt color mapping in the "view" command.

      case 'f': return nGlblFileColor ; break;
      case 'l': return nGlblLinkColor ; break;
      case 'h': return nGlblHeadColor ; break;
      case 'i': return nGlblHitColor  ; break;
      case 'a': return nGlblRepColor  ; break;
      case 'x': return nGlblExampColor; break;
      case 'e': return nGlblErrColor  ; break;
      case 'w': return nGlblWarnColor ; break;
      case 't': return nGlblTimeColor ; break;
      case 'p': return nGlblPreColor  ; break;

      // bright base colors
      case 'R': return b+2 ; break;
      case 'G': return b+4 ; break;
      case 'B': return b+8 ; break;
      case 'Y': return b+6 ; break;
      case 'C': return b+12; break;
      case 'M': return b+10; break;
      case 'V': return b+14; break;

      // dark base colors
      case 'r': return d+2 ; break;
      case 'g': return d+4 ; break;
      case 'b': return d+8 ; break;
      case 'y': return d+6 ; break;
      case 'c': return d+12; break;
      case 'm': return d+10; break;
      case 'v': return d+14; break;
   }

   // default:
   return -1;
}

// add to szLineBuf/szAttrBuf. uses szLineBuf2.
// flush line if '\n' is seen.
// consider output chaining.
int addColorText(char ccol, const char *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szLineBuf2, MAX_LINE_LEN, pszFormat, argList);
   szLineBuf2[MAX_LINE_LEN] = '\0';

   int iadd = strlen(szLineBuf);
   int nadd = strlen(szLineBuf2);

   if (strlen(szAttrBuf) != iadd) {
      pwarn("invalid attrib buffer content (%d/%d)\n",(int)strlen(szAttrBuf),iadd);
      memset(szAttrBuf, ' ', iadd);
      szAttrBuf[iadd] = '\0';
   }

   if (iadd + nadd + 10 > MAX_LINE_LEN) return 9;

   memcpy(szLineBuf+iadd, szLineBuf2, nadd);
          szLineBuf[iadd+nadd] = '\0';

   memset(szAttrBuf+iadd, ccol, nadd);
          szAttrBuf[iadd+nadd] = '\0';

   // flush to terminal or chain
   char *peol = strchr(szLineBuf, '\n');
   if (peol) {
      iadd = peol - szLineBuf;
      szLineBuf[iadd] = '\0';
      szAttrBuf[iadd] = '\0';
      if (chain.coldata)
         chain.addLine(szLineBuf, szAttrBuf);
      else
         printColorText(szLineBuf, szAttrBuf);
      szLineBuf[0] = '\0';
      szAttrBuf[0] = '\0';
   }

   return 0;
}

void printColorText(char *pszText, char *pszAttrib, bool bWithLF)
{
   // printf("\n\n\n--- pct.text/attr: ---\n%s\n", pszText);
   // printf("%s\n--- output: ---\n", pszAttrib);

   int nTextLen = strlen(pszText);
   int nAttrLen = strlen(pszAttrib);

   // if insufficient attribs provided, use no color
   if (nAttrLen < nTextLen) {
      oprintf("%s%s", pszText, bWithLF ? "\n":"");
      return;
   }

   int i1=0,i2=0;
   while (i1 < nTextLen)
   {
      // identify next phrase of same color
      uchar a1 = pszAttrib[i1];
      for (i2=1; i1+i2<nTextLen; i2++)
         if (pszAttrib[i1+i2] != a1)
            break;

      // dump next phrase with len i2.
      int ncolor = sfkMapAttrToColor(a1);
      setTextColor(ncolor);

      if (bGlblHtml)
         printHtml(pszText+i1, (int)i2);
      else
         oprintf("%.*s", (int)i2, pszText+i1);

      // step forward
      i1 += i2;
   }
   setTextColor(-1);
   if (bWithLF) {
      // putchar('\n');
      oprintf("\n"); // sfk1914
   }
}

char attribFromHumanColor(char *pszCol, char cDefault='i')
{
   // red -> dark red. Red or RED -> bright red.
   if (!mystricmp(pszCol, "red"))     return pszCol[0];
   if (!mystricmp(pszCol, "green"))   return pszCol[0];
   if (!mystricmp(pszCol, "blue"))    return pszCol[0];
   if (!mystricmp(pszCol, "yellow"))  return pszCol[0];
   if (!mystricmp(pszCol, "cyan"))    return pszCol[0];
   if (!mystricmp(pszCol, "magenta")) return pszCol[0];
   if (!mystricmp(pszCol, "def"))     return ' ';
   if (!mystricmp(pszCol, "default")) return ' ';
   if (!mystricmp(pszCol, "white"))   return (pszCol[0] == 'W') ? 'V':'v';

   // since 1.58 also supporting logical colors
   if (!mystricmp(pszCol, "err"))     return 'e';
   if (!mystricmp(pszCol, "warn"))    return 'w';
   if (!mystricmp(pszCol, "head"))    return 'h';
   if (!mystricmp(pszCol, "examp"))   return 'x';
   if (!mystricmp(pszCol, "file"))    return 'f';
   if (!mystricmp(pszCol, "hit"))     return 'i';
   if (!mystricmp(pszCol, "rep"))     return 'a';
   if (!mystricmp(pszCol, "pre"))     return 'p';
   if (!mystricmp(pszCol, "time"))    return 't';

   static bool btold = 0;
   if (!btold)
   {
      btold = 1;
      pwarn("unsupported color name: %s (try \"sfk help color\")\n", pszCol);
   }

   return cDefault;
}

bool bGlblCheckPrintX = 0;
bool bGlblDumpHelpRaw = 0;

int printx(const char *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);

   if (bGlblCheckPrintX)
      szPrintBuf1[sizeof(szPrintBuf1)-12] = '\xFF';

   ::vsnprintf(szPrintBuf1, sizeof(szPrintBuf1)-10, pszFormat, argList);
   szPrintBuf1[sizeof(szPrintBuf1)-10] = '\0';
 
   if (bGlblCheckPrintX)
      if (szPrintBuf1[sizeof(szPrintBuf1)-12] != '\xFF')
         fprintf(stderr, "printx overflow.1: %.30s ...\n", szPrintBuf1);

   if (bGlblDumpHelpRaw) {
      printf("%s", szPrintBuf1);
      return 0;
   }

   char *pszSrc = szPrintBuf1;
   int  iDst = 0;
   char nAttr = ' ';
   bool bResetOnLF = 0;
   bool bNoCol = 0;
   while (*pszSrc && (iDst < (int)sizeof(szPrintBuf2)-10))
   {
      if (bNoCol) {
         szPrintBuf2[iDst] = *pszSrc;
         szPrintAttr[iDst] = nAttr;
         pszSrc++;
         iDst++;
         continue;
      }
      if (!strncmp(pszSrc, "<nocol>", 7))
         { pszSrc += 7; bNoCol=1; continue; }
      if (pszSrc[0] == '\\' && pszSrc[1] == '<') {
         pszSrc += 2;
         szPrintBuf2[iDst] = '<';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "<help>", 6))  { pszSrc += 6; if (cs.usehelpcolor) cs.usecolor = 1; } else
      if (!strncmp(pszSrc, "<file>", 6))  { pszSrc += 6; nAttr = 'f'; } else
      if (!strncmp(pszSrc, "<head>", 6))  { pszSrc += 6; nAttr = 'h'; } else
      if (!strncmp(pszSrc, "<prefix>", 8)){ pszSrc += 8; nAttr = 'p'; } else
      if (!strncmp(pszSrc, "<examp>", 7)) { pszSrc += 7; nAttr = 'x'; } else
      if (!strncmp(pszSrc, "$$"    , 2))  {
         pszSrc += 2;
         szPrintBuf2[iDst] = '$';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "$"     , 1)) { pszSrc += 1; nAttr = 'h'; bResetOnLF = 1; } else
      if (!strncmp(pszSrc, "##"    , 2)) {
         pszSrc += 2;
         szPrintBuf2[iDst] = '#';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (pszSrc[0] == '\\' && pszSrc[1] == '*') {
         pszSrc += 2;
         szPrintBuf2[iDst] = '*';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (pszSrc[0] == '\\' && pszSrc[1] == glblWildChar) {
         pszSrc += 2;
         szPrintBuf2[iDst] = glblWildChar;
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "#"     , 1)) { pszSrc += 1; nAttr = 'x'; bResetOnLF = 1; } else
      if (!strncmp(pszSrc, "<hit>" , 5)) { pszSrc += 5; nAttr = 'i'; } else
      if (!strncmp(pszSrc, "<rep>" , 5)) { pszSrc += 5; nAttr = 'a'; } else
      if (!strncmp(pszSrc, "<err>" , 5)) { pszSrc += 5; nAttr = 'e'; } else
      if (!strncmp(pszSrc, "<warn>", 6)) { pszSrc += 6; nAttr = 'w'; } else
      if (!strncmp(pszSrc, "<time>", 6)) { pszSrc += 6; nAttr = 't'; } else
      if (!strncmp(pszSrc, "<def>" , 5)) { pszSrc += 5; nAttr = ' '; } else

      if (!strncmp(pszSrc, "[["    , 2)) {
         pszSrc += 2;
         szPrintBuf2[iDst] = '[';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      if (!strncmp(pszSrc, "]]"    , 2)) {
         pszSrc += 2;
         szPrintBuf2[iDst] = ']';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      // base colors are bright or dark depending on their first name char
      if (!mystrncmp(pszSrc, "[red]"    , 5)) { nAttr = pszSrc[1]; pszSrc += 5; } else
      if (!mystrncmp(pszSrc, "[green]"  , 7)) { nAttr = pszSrc[1]; pszSrc += 7; } else
      if (!mystrncmp(pszSrc, "[blue]"   , 6)) { nAttr = pszSrc[1]; pszSrc += 6; } else
      if (!mystrncmp(pszSrc, "[yellow]" , 8)) { nAttr = pszSrc[1]; pszSrc += 8; } else
      if (!mystrncmp(pszSrc, "[cyan]"   , 6)) { nAttr = pszSrc[1]; pszSrc += 6; } else
      if (!mystrncmp(pszSrc, "[magenta]", 9)) { nAttr = pszSrc[1]; pszSrc += 9; } else

      if (!strncmp(pszSrc, "[def]"    , 5)) { pszSrc += 5; nAttr = ' '; } else

      if (!strncmp(pszSrc, "[CR]"     , 4)) {
         pszSrc += 4;
         szPrintBuf2[iDst] = '\r';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      if (!strncmp(pszSrc, "[LF]"     , 4)) {
         pszSrc += 4;
         szPrintBuf2[iDst] = '\n';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      if (!strncmp(pszSrc, "<not>"    , 5)) {
         pszSrc += 5;
         szPrintBuf2[iDst] = glblNotChar;
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "<run>" , 5)) {
         pszSrc += 5;
         szPrintBuf2[iDst] = glblRunChar;
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "<wild>" , 6)) {
         pszSrc += 6;
         szPrintBuf2[iDst] = glblWildChar;
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "<sla>" , 5)) {
         pszSrc += 5;
         szPrintBuf2[iDst] = glblPathChar;
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else
      if (!strncmp(pszSrc, "<exp>" , 5)) {
         pszSrc += 5;
         #ifdef _WIN32
         memcpy(szPrintBuf2+iDst, "set", 3);
         memset(szPrintAttr+iDst, nAttr, 3);
         iDst += 3;
         #else
         memcpy(szPrintBuf2+iDst, "export", 6);
         memset(szPrintAttr+iDst, nAttr   , 6);
         iDst += 6;
         #endif
      }
      else
      if (!strncmp(pszSrc, "<shext>" , 7)) {
         pszSrc += 7;
         #ifdef _WIN32
         memcpy(szPrintBuf2+iDst, "bat", 3);
         memset(szPrintAttr+iDst, nAttr, 3);
         iDst += 3;
         #else
         memcpy(szPrintBuf2+iDst, "sh", 2);
         memset(szPrintAttr+iDst, nAttr, 2);
         iDst += 2;
         #endif
      }
      else
      if (!strncmp(pszSrc, "<nofo>" , 6)) {
         pszSrc += 6;
         #ifndef _WIN32
         memcpy(szPrintBuf2+iDst, "-nofo ", 6);
         memset(szPrintAttr+iDst, nAttr   , 6);
         iDst += 6;
         #endif
      }
      else {
         szPrintBuf2[iDst] = *pszSrc;
         szPrintAttr[iDst] = nAttr;
         if (*pszSrc == '\n' && bResetOnLF) {
            nAttr = ' ';
            szPrintAttr[iDst] = nAttr;
            bResetOnLF = 0;
         }
         pszSrc++;
         iDst++;
      }
   }
   szPrintBuf2[iDst] = '\0';

   if (bGlblCheckPrintX)
      if (iDst >= (int)sizeof(szPrintBuf2)-10)
         fprintf(stderr, "printx overflow.2: %.30s ...\n", szPrintBuf2);

   if (iGlblCollectCmd)
   {
      if (iGlblCollectCmd == 1)
      {
         void addHelpCmdLines(char *pszRaw, char *pszForm);
         addHelpCmdLines((char*)pszFormat, szPrintBuf2);
      }
   }
   else if (bGlblCollectHelp)
   {
      int chainAddLine(char *pszText, char *pszAttr, bool bSplitByLF);
      chainAddLine(szPrintBuf2, szPrintAttr, 1);
   }
   else
   {
      printColorText(szPrintBuf2, szPrintAttr, 0);
   }

   return 0;
}

// print whole chapter from main help text
int printHelp(const char *pszFormat, ...)
{
   char *pszBuf  = (char*)abBuf;
   int   iMaxBuf = sizeof(abBuf) - 100;

   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(pszBuf, iMaxBuf, pszFormat, argList);
   pszBuf[iMaxBuf] = '\0';

   /*
   file system
      sfk list       - list directory tree contents.
                       list latest, oldest or biggest files.
                       list changed or identical files.
      sfk filefind   - find files by filename
      sfk treesize   - show directory size statistics
   */

   cchar *pszind = "   ";

   char *psz1 = pszBuf;
   char *psz2 = 0;
   int   ilen = 0;
   for (; psz1 && *psz1; psz1=psz2)
   {
      if ((psz2 = strchr(psz1, '\n'))) {
         ilen = psz2 - psz1;
         psz2++; // skip LF
      } else {
         ilen = strlen(psz1);
      }
      char *pszLineEnd = psz1 + ilen;

      // process current line
      if (*psz1 != ' ') {
         // headline
         printx("%s<file>%.*s<def>\n",pszind,ilen,psz1);
         continue;
      }

      // split command entry
      char *pcmd=0, *pcmde=0, *ptext=0;

      pcmd = psz1;
      skipOver(&pcmd, " ");
      if (*pcmd=='\r' || *pcmd=='\n') {
         // no command name, but empty line
         printx("\n");
         continue;
      }
      if (pcmd > psz1+3) {
         // no command name, but continuation line
         printx("%s%.*s\n",pszind,ilen,psz1);
         continue;
      }

      ptext = strstr(pcmd, "- ");
      if (ptext) {
         // found "-" before info text
         pcmde = ptext;
         while (pcmde > pcmd && pcmde[-1] == ' ')
            pcmde--;
         // show name and text in two colors
         int icmdlen = pcmde - pcmd;
         int itxtlen = pszLineEnd - pcmde;
         printx("%s   #%.*s<def>%.*s\n",pszind,icmdlen,pcmd,itxtlen,pcmde);
      } else {
         // command name without info text
         printx("%s$%.*s<def>\n",pszind,ilen,psz1);
         continue;
      }
   }

   return 0;
}

// - - - lists and maps

ListEntry::ListEntry( )  { pClNext = pClPrevious = 0; data = 0; }
ListEntry::~ListEntry( ) { pClNext = pClPrevious = 0; data = 0; }

List::List( )  { reset(); }
List::~List( ) { reset(); }

int List::size( )
{
   int icount = 0;
   ListEntry *p = pClFirst;
   while (p) {
      icount++;
      p = p->pClNext;
   }
   return icount;
}

// NOTE: caller MUST FREE all list elements before reset()!
void List::reset( ) {
   pClFirst = pClLast = 0;
}

void List::add(ListEntry* pNew)
{
   if (!pClFirst)
   {
      // yet empty list:
      pClFirst = pClLast = pNew;
      pNew->pClNext = pNew->pClPrevious = 0;
      return;
   }

   // append node at end of list:
   pClLast->pClNext   = pNew;
    pNew->pClPrevious = pClLast;
    pNew->pClNext     = 0;
   pClLast            = pNew;
}

void List::addAsFirst(ListEntry* pNew)
{
   // yet empty list?
   if (!pClFirst)
   {
      pClFirst = pClLast = pNew;
      pNew->pClNext = pNew->pClPrevious = 0;
      return;
   }

   // make node new front of list:
   ListEntry *n2 = pClFirst;
   n2->pClPrevious      = pNew;
    pNew->pClPrevious   = 0;
    pNew->pClNext       = n2;
   pClFirst             = pNew;
}

void List::addAfter(ListEntry *pAfter, ListEntry *pNew)
{
   ListEntry *pNext = pAfter->pClNext;  // might be 0

   pAfter->pClNext   = pNew;
   pNew->pClPrevious = pAfter;
   pNew->pClNext     = pNext;

   if (pNext)
      pNext->pClPrevious = pNew;
   else
      pClLast  = pNew;
}

void List::remove(ListEntry* pRemove)
{
   ListEntry *pPrevious = pRemove->pClPrevious;  // might be 0
   ListEntry *pNext     = pRemove->pClNext;      // might be 0

   pRemove->pClNext = pRemove->pClPrevious = 0;

   if (!pPrevious)   // if 'pRemove' at start of list
   {
      if ((pClFirst = pNext))    // new list start becomes pNext ...
         pNext->pClPrevious = 0; // ... and if pNext exists, adjust it,
      else
         pClLast  = 0;           // else list is empty.
   }
   else
   {
      // at least a 'pClPrevious' is given.
      if ((pPrevious->pClNext = pNext))   // let pPrevious' 'pClNext' ptr bypass 'pRemove' ...
         pNext->pClPrevious = pPrevious;  // ... and if pNext exists, adjust it,
      else
         pClLast  = pPrevious;            // else set new listend.
   }
}

KeyMap::KeyMap( ) {
   mtklog(("keymap ctr %p", this));
   wipe();
}

KeyMap::~KeyMap( ) {
   mtklog(("keymap dtr %p", this));
   reset();
   wipe();
}

void KeyMap::wipe( ) {
   nClArrayAlloc = 0;
   nClArrayUsed = 0;
   apClKey  = 0;
   apClVal  = 0;
   bClCase  = 1;
   bClRev   = 0;
}

void KeyMap::reset( )
{
   for (int i=0; i<nClArrayUsed; i++)
      delete [] apClKey[i];
   if (apClKey) delete [] apClKey;
   if (apClVal) delete [] apClVal;
   wipe();
}

void KeyMap::setcase(bool bYesNo)    { bClCase = bYesNo; }
void KeyMap::setreverse(bool bYesNo) { bClRev = bYesNo;  }

int KeyMap::expand(int nSoMuch)
{__
   // expand the key array
   char **apTmp1 = new char*[nClArrayAlloc+nSoMuch];
   if (!apTmp1) return 9;
   if (apClKey) {
      memcpy(apTmp1, apClKey, nClArrayUsed*sizeof(char*));
      delete [] apClKey;
   }
   apClKey = apTmp1;

   // expand the value array
   void **apTmp2 = new void*[nClArrayAlloc+nSoMuch];
   if (!apTmp2) return 9;
   if (apClVal) {
      memcpy(apTmp2, apClVal, nClArrayUsed*sizeof(void*));
      delete [] apClVal;
   }
   apClVal = apTmp2;

   // set new alloc size
   nClArrayAlloc += nSoMuch;

   return 0;
}

// rc =0:found_and_index_set
// rc <0:insert_before_index
// rc >0:insert_after_index
int KeyMap::bfind(char *pkey, int &rindex)
{
   // binary search for key, or insert position
   uint nbot=0,ndist=0,nhalf=0,imid=0;
   uint ntop=nClArrayUsed; // exclusive
   char  *ptmp=0;
   int    ncmp=-1;   // if empty, insert before index 0

   while (1)
   {
      if (nbot > ntop) // shouldn't happen
         { perr(0, "int. 187281850"); ncmp=-1; break; }

      ndist = ntop - nbot;
      // mtklog(("dist %d bot %d top %d",ndist,nbot,ntop));
      if (ndist == 0) break; // nothing left
      nhalf = ndist >> 1;
      imid  = nbot + nhalf;
      ptmp  = apClKey[imid];
      ncmp  = bClCase ? strcmp(pkey, ptmp) : mystricmp(pkey, ptmp);
      if (bClRev) ncmp = 0 - ncmp;
 
      if (ncmp < 0) {
         // select lower half, if any
         // mtklog((" take lower %lxh %lxh %d",nval,ntmp,imid));
         if (ntop == imid) break; // safety
         ntop = imid;
      }
      else
      if (ncmp > 0) {
         // select upper half, if any
         // mtklog((" take upper %lxh %lxh %d",nval,ntmp,imid));
         if (nbot == imid+1) break; // required
         nbot = imid+1;
      } else {
         // straight match
         mtklog(("%d = indexof(%.30s) used=%u",imid,pkey,nClArrayUsed));
         break; // found
      }
   }

   rindex = imid;
   return ncmp;
}

int KeyMap::put(char *pkey, void *pval)
{
   // assure space for insertion, incl. safety
   if (nClArrayUsed >= nClArrayAlloc-10) {
      if (nClArrayAlloc == 0) {
         if (expand(20)) return 9;
      } else {
         if (expand(nClArrayAlloc)) return 9;
      }
   }

   // search for key or insert position:
   int imid=0;
   int  nrc = bfind(pkey, imid);

   if (nrc)
   {
      // no match, insert new entry:
      // if ncmp <= 0: before imid
      // if ncmp  > 0: after imid
      if (nrc > 0) imid++;
 
      // before imid as it is now
      uint ntomove = nClArrayUsed - imid;
      memmove(&apClKey[imid+1], &apClKey[imid], sizeof(char*) * ntomove);
      memmove(&apClVal[imid+1], &apClVal[imid], sizeof(void*) * ntomove);
      apClKey[imid] = strdup(pkey); // is COPIED.
      nClArrayUsed++;
   }

   // target position is now imid.
   apClVal[imid] = pval;
   // if there was an old pval, it is replaced.

   return 0; // done
}

void *KeyMap::get(char *pkey, int *poutidx)
{
   int imid=0;
   int  nrc = bfind(pkey, imid);
   // even on miss return last comparison index
   // to allow caller to find nearby words.
   if (poutidx) *poutidx = imid;
   if (nrc) return 0;
   if (imid < 0 || imid >= nClArrayUsed)
      {  perr("int. 187281919"); return 0; }
   return apClVal[imid]; // if any
}

#if 0

// old proven code, may produce compiler warning
int KeyMap::putnum(char *pkey, num nval)
{
   void *p = (void*)nval;
   return put(pkey, p);
}

num KeyMap::getnum(char *pkey, int *poutidx)
{
   void *p = get(pkey, poutidx);
   return (num)p;
}

#else

/*
   sfk199 avoid compiler warning by a union:
   the KeyMap stores a      key -> pobj mapping,
   but can also be used for key -> int mapping.
   internally it stores just a void *p.
   in case of a 32-bit compile this limits
   the stored number to 32 bits also.
*/
typedef union
{
   void *pval;
   num   nval;
   uint  uval;
} sfkNumPtr_t;

int KeyMap::putnum(char *pkey, num nval)
{
   sfkNumPtr_t u;
   memset(&u, 0, sizeof(u));
   if (sizeof(u.pval) <= 4)
      u.uval = (uint)nval;
   else
      u.nval = nval;
   void *p = u.pval;
   return put(pkey, p);
}

num KeyMap::getnum(char *pkey, int *poutidx)
{
   sfkNumPtr_t u;
   memset(&u, 0, sizeof(u));
   void *p = get(pkey, poutidx);
   u.pval = p;
   if (sizeof(u.pval) <= 4)
      return (num)u.uval;
   return (num)u.nval;
}

#endif

bool KeyMap::isset(char *pkey) {
   int imid=0;
   return bfind(pkey, imid) ? 0 : 1;
}

int KeyMap::remove(int imid)
{
   // remove entry at imid
   delete [] apClKey[imid];
   if (imid < nClArrayUsed) {
      uint ntomove = nClArrayUsed - imid - 1;
      if (ntomove > 0) { // fix: 1763: missing check
         memmove(&apClKey[imid+0], &apClKey[imid+1], sizeof(char*) * ntomove);
         memmove(&apClVal[imid+0], &apClVal[imid+1], sizeof(void*) * ntomove);
      }
   }
   nClArrayUsed--;
   return 0;
}

int KeyMap::remove(char *pkey)
{
   int imid=0;
   int  nrc = bfind(pkey, imid);
   if (nrc) return 1; // no such key
   return remove(imid);
}

int KeyMap::size( ) {
   return nClArrayUsed;
}

void *KeyMap::iget(int nindex, char **ppkey)
{
   if (nindex < 0 || nindex >= nClArrayUsed)
      { if (ppkey) *ppkey = 0; return 0; }
   if (ppkey) *ppkey = apClKey[nindex];
   return apClVal[nindex];
}

num KeyMap::igetnum(int nindex, char **ppkey)
{
   void *pval = iget(nindex, ppkey);
   return (num)pval;
}

// - - - keymap access with num keys: - -  -

// so far, the nums are simply converted into
// a hex string, prefixed by a non-printable 0x1.

inline void numToKey(num nkey, char *pbuf) {
   pbuf[0] = '\x01';
   pbuf[1] = '\0';
   numtohex(nkey, 16, pbuf+1);
}

int  KeyMap::put   (num nkey, void *pvalue) {
   char szBuf[100];
   numToKey(nkey, szBuf);
   return put(szBuf, pvalue);
}

void *KeyMap::get   (num nkey) {
   char szBuf[100];
   numToKey(nkey, szBuf);
   return get(szBuf);
}

bool  KeyMap::isset (num nkey) {
   char szBuf[100];
   numToKey(nkey, szBuf);
   return isset(szBuf);
}

int  KeyMap::remove(num nkey) {
   char szBuf[100];
   numToKey(nkey, szBuf);
   return remove(szBuf);
}

num hextonum(char *psz)
{
   #ifdef _MSC_VER
   return _strtoui64(psz, 0, 0x10);
   #else
   return strtoull(psz, 0, 0x10);
   #endif
}

num KeyMap::igetnum(int nindex, num *pkey) {
   void *pval = iget(nindex, pkey);
   return (num)pval;
}

void *KeyMap::iget  (int nindex, num *pkey) {
   char *pszkey = 0;
   void *pres = iget(nindex, &pszkey);
   if (pkey) {
      if (*pszkey == '\x01') {
         num nkey = hextonum(pszkey+1);
         *pkey = nkey;
      } else {
         perr("wrong keymap key for index %d", nindex);
         *pkey = 0;
      }
   }
   return pres;
}

// - - - keymap access with num keys end - -  -

StringMap::StringMap( )  {
   mtklog(("strmap ctr %p", this));
}

StringMap::~StringMap( ) {
   mtklog(("strmap dtr %p", this));
   reset();
}

void StringMap::reset( )
{__
   mtklog(("strmap reset %p", this));
   for (int i=0; i<size(); i++)
      if (apClVal[i])
         delete [] (char*)apClVal[i];
   KeyMap::reset();
}

int StringMap::put(char *pkey, char *pvalin)
{
   // COPY the input string, but also accept NULL.
   char *pcopy = pvalin ? strdup(pvalin) : 0;

   mtklog(("strmap put this=%p %p key=%s",this,pcopy,pkey));

   // is the key is stored already?
   int imid=0;
   int  nrc = bfind(pkey, imid);
   if (!nrc)
   {
      // yes: if there is a value, delete it
      if (apClVal[imid])
         delete [] (char*)apClVal[imid];
      // and set new value, also accepting NULL.
      apClVal[imid] = pcopy;
      return 0;
   }

   // else add a copy
   return KeyMap::put(pkey, pcopy);
}

char *StringMap::get(char *pkey, char *pszOptDef)
{
   char *pres = (char*)KeyMap::get(pkey);
   if (!pres && pszOptDef) pres = pszOptDef;
   return pres;
}

int StringMap::remove(char *pkey)
{
   int imid=0;
   int  nrc = bfind(pkey, imid);
   if (!nrc && apClVal[imid])
      delete [] (char*)apClVal[imid];
   return KeyMap::remove(pkey);
}

char *StringMap::iget(int nindex, char **ppkey)
{
   char *pkey = 0;
   char *pval = (char*)KeyMap::iget(nindex, &pkey);
   if (ppkey) *ppkey = pkey;
   return pval;
}

int StringMap::put(num nkey, char *pvalin)
{
   // pvalin is dup'ed in the called put()

   mtklog(("strmap nput this=%p %lxh %p",this,(uint)nkey,pvalin));

   char szBuf[100];
   szBuf[0] = '\0';
   numtohex(nkey, 16, szBuf);

   return StringMap::put(szBuf, pvalin);
}

char *StringMap::get(num nkey)
{
   char szBuf[100];
   szBuf[0] = '\0';
   numtohex(nkey, 16, szBuf);
   return (char*)get(szBuf);
}

char *StringMap::iget(int nindex, num *pkey)
{
   char *pszkey = 0;
   void *pres = iget(nindex, &pszkey);
   if (pkey) {
      num nkey = hextonum(pszkey);
      *pkey = nkey;
   }
   return (char*)pres;
}

int StringMap::remove(num nkey)
{
   char szBuf[100];
   szBuf[0] = '\0';
   numtohex(nkey, 16, szBuf);

   int imid = 0;
   int nrc  = bfind(szBuf, imid);

   if (nrc)
      return 1;

   // cleanup value string
   if (apClVal[imid])
      delete [] (char*)apClVal[imid];

   // cleanup key?
   KeyMap::remove(imid); // FIX: 1763: missing

   return 0;
}

AttribStringMap::AttribStringMap( )  { }
AttribStringMap::~AttribStringMap( ) { }

char *AttribStringMap::mixdup(char *ptext, char *pattr)
{
   int nlen1 = strlen(ptext);
   int nlen2 = strlen(pattr);

   // mixed = TEXT + ZEROTERM + ATTR + ZEROTERM + MAGIC
   int nmix  = nlen1 + 1 + nlen2 + 1 + 1;

   char *pmix = new char[nmix];

   memcpy(pmix, ptext, nlen1);
   pmix[nlen1] = '\0';

   memcpy(pmix+nlen1+1, pattr, nlen2);
   pmix[nlen1+1+nlen2] = '\0';

   // to allow consistency checks:
   pmix[nlen1+1+nlen2+1] = (char)0xFF;

   return pmix;
}

int AttribStringMap::demix(char *pmixed, char **pptext, char **ppattr)
{
   // mixed = TEXT + ZEROTERM + ATTR + ZEROTERM + MAGIC
   char *ptext  = pmixed;
   int nlen1   = strlen(ptext);
   char *pattr  = ptext + nlen1 + 1;
   int nlen2   = strlen(pattr);
   char *pmagic = pattr + nlen2 + 1;
   if (*pmagic != (char)0xFF)
      return 9+perr("color string failure, %p \"%.10s\" \"%.10s\" \"%.10s\"", pmixed, pmixed, ptext, pattr);
   *pptext = ptext;
   *ppattr = pattr;
   return 0;
}

int AttribStringMap::put(char *pkey, char *ptext, char *pattr)
{__
   char *pmix = mixdup(ptext, pattr);
   // mtklog(("asmap::put \"%s\" \"%s\" \"%s\"",pkey,ptext,pattr));
   // the string is dup'ed already,
   // place ptr straight into the keymap:
   return KeyMap::put(pkey, pmix);
}

int AttribStringMap::put(num nkey, char *ptext, char *pattr)
{__
   char *pmix = mixdup(ptext, pattr);
   // mtklog(("asmap::put %d \"%s\" \"%s\"",(int)nkey,ptext,pattr));
   // the string is dup'ed already,
   // place ptr straight into the keymap:
   return KeyMap::put(nkey, pmix);
}

char *AttribStringMap::get(char *pkey, char **ppattr)
{
   char *pmix = StringMap::get(pkey);
   if (!pmix) return 0;

   char *ptext=0,*pattr=0;
   if (demix(pmix, &ptext, &pattr)) return 0;

   *ppattr = pattr;
   return ptext;
}

char *AttribStringMap::iget(int nindex, char **ppkey, char **ppattr)
{
   char *pkey = 0;
   char *pmix = StringMap::iget(nindex, &pkey);
   if (!pmix) return 0;

   char *ptext=0,*pattr=0;
   if (demix(pmix, &ptext, &pattr)) return 0;

   if (ppkey) *ppkey = pkey;
   *ppattr = pattr;
   return ptext;
}

char *AttribStringMap::get(num nkey, char **ppattr)
{
   char *pmix = StringMap::get(nkey);
   if (!pmix) return 0;

   char *ptext=0,*pattr=0;
   if (demix(pmix, &ptext, &pattr)) return 0;

   *ppattr = pattr;
   return ptext;
}

char *AttribStringMap::iget(int nindex, num *pkey, char **ppattr)
{
   num nkey = 0;
   char *pmix = StringMap::iget(nindex, &nkey);
   if (!pmix) return 0;

   char *ptext=0,*pattr=0;
   if (demix(pmix, &ptext, &pattr)) return 0;

   if (pkey) *pkey = nkey;
   *ppattr = pattr;
   return ptext;
}

StringPipe::StringPipe()
{
   nReadIndex = 0;
}

bool StringPipe::eod()
{
   return (nReadIndex >= numberOfEntries());
}

char *StringPipe::read(char **ppAttr)
{
   if (nReadIndex < numberOfEntries()) {
      char *psz   = clText.getEntry(nReadIndex, __LINE__);
      char *pattr = clAttr.getEntry(nReadIndex, __LINE__);
      // printf("PIPE.READ %d = %s\n",nReadIndex,psz);
      nReadIndex++;
      if (ppAttr)
         *ppAttr = pattr;
      return psz;
   }
   return 0; // EOD
}

int StringPipe::setEntry(int iIndex, char *psz, char *pAttr)
{
   if (clAttr.setEntry(iIndex, pAttr)) return 9;
   return clText.setEntry(iIndex, psz);
}

int StringPipe::addEntry(char *psz, char *pAttr)
{
   if (clAttr.addEntry(pAttr)) return 9;
   return clText.addEntry(psz);
}

void StringPipe::resetEntries()
{
   clText.resetEntries();
   clAttr.resetEntries();
}

char *StringPipe::getEntry(int nIndex, int nLine, char **ppAttr)
{
   char *ptext = clText.getEntry(nIndex, nLine);
   char *pattr = clAttr.getEntry(nIndex, nLine);
   if (ppAttr) *ppAttr = pattr;
   return ptext;
}

void StringPipe::resetPipe()
{
   resetEntries();
   nReadIndex = 0;
}

void StringPipe::dump(cchar *pszTitle)
{
   printf("[pipe %s contents (%d lines):]\n",pszTitle,numberOfEntries());
   for (int i=0; i<numberOfEntries(); i++)
      printf("[   \"%s\"]\n",getEntry(i,__LINE__));
}

StringTable::StringTable() {
   nClArraySize = 0;
   nClArrayUsed = 0;
   apClArray    = 0;
}

StringTable::~StringTable() {
   resetEntries();
}

void StringTable::dump(int nIndent) {
   printf("] %.*sstringtable %p, %d entries:\n",nIndent,pszGlblBlank,this,nClArrayUsed);
   for (int i=0; i<nClArrayUsed; i++) {
      printf("]   %.*s%s\n", nIndent,pszGlblBlank,apClArray[i] ? apClArray[i] : "<null>");
   }
}

void StringTable::resetEntries() {
   if (apClArray) {
      for (int i=0; i<nClArrayUsed; i++) {
         if (apClArray[i]) delete [] apClArray[i];
         apClArray[i] = 0;
      }
   }
   nClArrayUsed = 0;
   if (apClArray)
      delete [] apClArray;
   apClArray = 0;
   nClArraySize = 0;
}

int StringTable::numberOfEntries() {
   return nClArrayUsed;
}

bool StringTable::isSet(int iIndex) {
   if (iIndex < 0)
      { pwarn("illegal index: %d\n", iIndex); return 0; }
   return (iIndex < nClArrayUsed) ? 1 : 0;
}

int StringTable::expand(int nSoMuch) {
   char **apTmp = new char*[nClArraySize+nSoMuch];
   if (!apTmp) return 9;
   if (apClArray) {
      memcpy(apTmp, apClArray, nClArraySize*sizeof(char*));
      delete [] apClArray;
   }
   apClArray = apTmp;
   nClArraySize += nSoMuch;
   return 0;
}

int StringTable::addEntry(char *psz, int nAtPos, char **ppCopy)
{
   char *pCopy = 0;
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }
   if (nAtPos != -1) {
      for (int i=nClArrayUsed; i>nAtPos; i--)
         apClArray[i] = apClArray[i-1];
      pCopy = psz ? strdup(psz) : 0;
      apClArray[nAtPos] = pCopy;
      nClArrayUsed++;
   } else {
      pCopy = psz ? strdup(psz) : 0;
      apClArray[nClArrayUsed++] = pCopy;
   }
   if (ppCopy) *ppCopy = pCopy;
   return 0;
}

int StringTable::removeEntry(int nAtPos) {
   if (nAtPos < 0 || nAtPos >= nClArrayUsed)
      return 9;
   if (apClArray[nAtPos]) delete [] apClArray[nAtPos];
   for (int i=nAtPos; i<nClArrayUsed-1; i++)
      apClArray[i] = apClArray[i+1];
   apClArray[nClArrayUsed-1] = 0; // just in case
   nClArrayUsed--;
   return 0;
}

int StringTable::addEntryPrefixed(char *psz, char cPrefix) {
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }
   // create extended copy with prefix at beginning
   int nLen = strlen(psz);
   char *pszCopy = new char[nLen+2];
   pszCopy[0] = cPrefix;
   strcpy(pszCopy+1, psz);
   // add this copy
   apClArray[nClArrayUsed++] = pszCopy;
   if (cs.debug) printf("]  stringtable %p added %s, have %d\n",this,pszCopy,nClArrayUsed);
   return 0;
}

int StringTable::setEntry(int nIndex, char *psz) {
   if (nIndex >= nClArrayUsed)
      return 9+perr("illegal set index: %d\n", nIndex);
   if (apClArray[nIndex])
      delete [] apClArray[nIndex];
   apClArray[nIndex] = strdup(psz);
   return 0;
}

int StringTable::setEntryPrefixed(int nIndex, char *psz, char cPrefix) {
   if (nIndex >= nClArrayUsed)
      return 9+perr("illegal set index: %d\n", nIndex);
   // create extended copy with prefix at beginning
   int nLen = strlen(psz);
   char *pszCopy = new char[nLen+2];
   pszCopy[0] = cPrefix;
   strcpy(pszCopy+1, psz);
   // delete old entry, if any
   if (apClArray[nIndex])
      delete [] apClArray[nIndex];
   // set new copy
   apClArray[nIndex] = pszCopy;
   if (cs.debug) printf("]  stringtable %p set %s, have %d\n",this,pszCopy,nClArrayUsed);
   return 0;
}

char *StringTable::getEntry(int nIndex, int nTraceLine) {
   if (nIndex >= 0 && nIndex < nClArrayUsed)
      return apClArray[nIndex];
   perr("illegal StringTable index: %d tline=%d used=%d\n", nIndex, nTraceLine, nClArrayUsed);
   return 0;
}

int StringTable::find(char *psz) {
   for (int i=0; i<nClArrayUsed; i++)
      if (apClArray[i] && !strcmp(apClArray[i], psz))
         return i;
   return -1;
}

Array::Array(const char *pszID) {
   nClRowsSize = 0;
   nClRowsUsed = 0;
   apClRows    = 0;
   nClCurRow   = 0;
   pszClID     = pszID;
}

Array::~Array() {
   reset();
}

void Array::dump() {
   printf("] array %s dump:\n",pszClID);
   for (int iRow=0; iRow<nClRowsUsed; iRow++) {
      StringTable *pRow = apClRows[iRow];
      printf("]   row %p:\n",pRow);
      pRow->dump(5);
   }
}

int Array::expand(int nSoMuch) {
   StringTable **apTmp = new StringTable*[nClRowsSize+nSoMuch];
   if (!apTmp) return 9;
   if (apClRows) {
      memcpy(apTmp, apClRows, nClRowsSize*sizeof(StringTable*));
      delete [] apClRows;
   }
   apClRows = apTmp;
   nClRowsSize += nSoMuch;
   return 0;
}

// make sure at least one row exists
int Array::ensureBase( ) {
   if (!nClRowsSize) {
      // if (cs.debug) printf("] array %p crt initial\n",this);
      // create initial row, for one-dimensional mode
      if (expand(1)) return 9;
      StringTable *pFirst = new StringTable();
      if (!pFirst) return 9;
      apClRows[nClRowsUsed++] = pFirst;
   }
   return 0;
}

// public: remove everything
void Array::reset() {
   // if (cs.debug) printf("] array %p RESET\n",this);
   for (int i=0; i<nClRowsUsed; i++) {
      delete apClRows[i];
      apClRows[i] = 0;
   }
   nClRowsUsed = 0;
   if (apClRows) {
      delete [] apClRows;
      apClRows = 0;
   }
   nClRowsSize = 0;
   nClCurRow = 0; // [22261] FIX sfk196: missing, crash on chaining
}

// public: return number of columns in current row
int Array::numberOfEntries() {
   if (ensureBase()) return 0;
   return apClRows[nClCurRow]->numberOfEntries();
}

int Array::numberOfEntries(int lRow) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow > nClRowsUsed)
      return 9+perr("%s: illegal row %d\n",pszClID,lRow);
   return apClRows[lRow]->numberOfEntries();
}

// public: add string object to current row
int Array::addString(char *psz) {
   if (ensureBase()) return 0;
   if (cs.debug) printf("] array %s: add to row %d entry %s. [%d rows total]\n",pszClID,nClCurRow,psz,nClRowsUsed);
   int lRC = apClRows[nClCurRow]->addEntryPrefixed(psz, 's');
   return lRC;
}

int Array::addString(int lRow, char *psz) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow > nClRowsUsed)
      return 9+perr("%s: illegal row %d\n",pszClID,lRow);
   if (cs.debug) printf("] array %s: add to row %d entry %s. [%d rows total]\n",pszClID,lRow,psz,nClRowsUsed);
   int lRC = apClRows[lRow]->addEntryPrefixed(psz, 's');
   return lRC;
}

int Array::addNull(int lRow) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow > nClRowsUsed)
      return 9+perr("%s: illegal row %d\n",pszClID,lRow);
   if (cs.debug) printf("] array %s: add NULL to row %d. [%d rows total]\n",pszClID,lRow,nClRowsUsed);
   int lRC = apClRows[lRow]->addEntry(0);
   return lRC;
}

int Array::setString(int lRow, int nIndex, char *psz) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow > nClRowsUsed)
      return 9+perr("%s: illegal row %d\n",pszClID,lRow);
   if (cs.debug) printf("] array %s: set row %d:%d entry %s. [%d rows total]\n",pszClID,lRow,nIndex,psz,nClRowsUsed);
   int lRC = apClRows[lRow]->setEntryPrefixed(nIndex, psz, 's');
   return lRC;
}

// public: get string object from current row
char *Array::getString(int nIndex) {
   if (ensureBase()) return 0;
   char *pszRaw = apClRows[nClCurRow]->getEntry(nIndex, __LINE__);
   if (!pszRaw)
      return 0; // NULL entry
   if (pszRaw[0] != 's') { perr("%s: no string entry type at %u %u\n", pszClID, nClCurRow, nIndex); return 0; }
   return pszRaw+1;
}

char *Array::getString(int lRow, int nIndex) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow >= nClRowsUsed) { perr("%s: illegal row %d\n",pszClID,lRow); return 0; }
   char *pszRaw = apClRows[lRow]->getEntry(nIndex, __LINE__);
   if (!pszRaw)
      return 0; // NULL entry
   if (pszRaw[0] != 's') { perr("%s: no string entry type at %u %u\n", pszClID, lRow, nIndex); return 0; }
   return pszRaw+1;
}

// public: add integer to current row, internally encoded as string
int Array::addLong(int nValue, int nTraceLine) {
   if (ensureBase()) return 0;
   if (cs.debug) printf("] array %s: add to row %d entry %d [tline %d]\n",pszClID,nClCurRow,nValue,nTraceLine);
   char szBuf[100];
   uint uValue = (uint)nValue;
   // char *_ultoa( unsigned int value, char *string, int radix );
   #ifdef _WIN32
   _ultoa(uValue, szBuf, 16);
   #else
   sprintf(szBuf, "%x", uValue);
   #endif
   return apClRows[nClCurRow]->addEntryPrefixed(szBuf, 'i');
}

int Array::addLong(int lRow, int nValue, int nTraceLine) {
   if (ensureBase()) return 0;
   if (cs.debug) printf("] array %s: add to row %d entry %d [tline %d]\n",pszClID,lRow,nValue,nTraceLine);
   if (lRow < 0 || lRow > nClRowsUsed) return 9+perr("%s: illegal row %d\n",pszClID,lRow);
   char szBuf[100];
   uint uValue = (uint)nValue;
   #ifdef _WIN32
   _ultoa(uValue, szBuf, 16);
   #else
   sprintf(szBuf, "%x", uValue);
   #endif
   return apClRows[lRow]->addEntryPrefixed(szBuf, 'i');
}

int Array::getLong(int nIndex) {
   if (ensureBase()) return 0;
   char *pszRaw = apClRows[nClCurRow]->getEntry(nIndex, __LINE__);
   if (!pszRaw)
      return 0;
   if (pszRaw[0] != 'i') { perr("no long entry type at row %u col %u\n", nClCurRow, nIndex); return 0; }
   // unsigned int strtoul( const char *nptr, char **endptr, int base );
   uint uValue = strtoul(pszRaw+1, 0, 16);
   return (int)uValue;
}

int Array::getLong(int lRow, int nIndex, int nTraceLine) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow >= nClRowsUsed) { perr("%s: illegal row %d\n",pszClID,lRow); return 0; }
   char *pszRaw = apClRows[lRow]->getEntry(nIndex, nTraceLine);
   if (!pszRaw)
      return 0;
   if (pszRaw[0] != 'i') { perr("no long entry type at row %u col %u\n", lRow, nIndex); return 0; }
   // unsigned int strtoul( const char *nptr, char **endptr, int base );
   uint uValue = strtoul(pszRaw+1, 0, 16);
   return (int)uValue;
}

int Array::setLong(int lRow, int nIndex, int nValue, int nTraceLine) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow >= nClRowsUsed) { perr("%s: illegal row %d\n",pszClID,lRow); return 0; }
   char szBuf[100];
   uint uValue = (uint)nValue;
   // char *_ultoa( unsigned int value, char *string, int radix );
   #ifdef _WIN32
   _ultoa(uValue, szBuf, 16);
   #else
   sprintf(szBuf, "%x", uValue);
   #endif
   return apClRows[lRow]->setEntryPrefixed(nIndex, szBuf, 'i');
}

// public: create new row. shouldn't be called for first row.
int Array::addRow(int nTraceLine) {
   int nOldRows = nClRowsSize;
   if (ensureBase()) return 0;
   // if anyone calls this before adding first data, accept it.
   if (!nOldRows)
      return 0;
   // for all further rows, check if row table is full.
   // if so, expand in exponential steps.
   if (nClRowsUsed == nClRowsSize) {
      if (expand(nClRowsSize)) return 9;
   }
   // finally, add new row object
   StringTable *pFirst = new StringTable();
   if (!pFirst) return 9;
   apClRows[nClRowsUsed++] = pFirst;
   nClCurRow = nClRowsUsed-1;
   if (cs.debug) printf("array %s extended to %d rows [tln %d]\n",pszClID,nClRowsUsed,nTraceLine);
   return 0;
}

// public: select current row
int Array::setRow(int iCurRow, int nTraceLine) {
   if (ensureBase()) return 9;
   if (iCurRow < 0 || iCurRow >= nClRowsUsed)
      return 9+perr("%s: illegal row index: %d on setRow, tline %d\n",pszClID,iCurRow,nTraceLine);
   nClCurRow = iCurRow;
   if (cs.debug) printf("array %s setrow %d\n",pszClID,nClCurRow);
   return 0;
}

// public: tell if index in current row is set, used for loops
bool Array::isSet(int iIndex) {
   if (ensureBase()) return 0;
   return apClRows[nClCurRow]->isSet(iIndex);
}

// public: tell if index in current row is set with a string
bool Array::isStringSet(int iIndex) {
   if (ensureBase()) return 0;
   if (!apClRows[nClCurRow]->isSet(iIndex)) {
      // if (cs.debug) printf("] %s: no string set at %d:%d\n",pszClID,nClCurRow,iIndex);
      return 0;
   }
   char *pszRaw = apClRows[nClCurRow]->getEntry(iIndex, __LINE__);
   // printf("xxx %s:%d:%d %s\n",pszClID,nClCurRow,iIndex,pszRaw);
   if (!pszRaw) {
      if (cs.debug) {
         printf("error: no entry at index %d, table %p, within array %p\n",iIndex,apClRows[nClCurRow],this);
         apClRows[nClCurRow]->dump();
      }
      return 0;
   }
   if (pszRaw[0] != 's') {
      perr("no string type at %s:%d:%d\n",pszClID,nClCurRow,iIndex);
      return 0;
   }
   return 1;
}

bool Array::isStringSet(int lRow, int iIndex) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow >= nClRowsUsed) { perr("%s: illegal row %d\n",pszClID,lRow); return 0; }
   if (!apClRows[lRow]->isSet(iIndex))
      return 0;
   char *pszRaw = apClRows[lRow]->getEntry(iIndex, __LINE__);
   if (!pszRaw) {
      if (cs.debug) {
         printf("error: no entry at index %d, table %p, within array %p\n",iIndex,apClRows[lRow],this);
         apClRows[lRow]->dump();
      }
      return 0;
   }
   return (pszRaw[0] == 's') ? 1 : 0;
}

// public: tell if index in current row is set with a int
bool Array::isLongSet(int lRow, int iIndex) {
   if (ensureBase()) return 0;
   if (lRow < 0 || lRow >= nClRowsUsed) { perr("%s: illegal row %d\n",pszClID,lRow); return 0; }
   if (!apClRows[lRow]->isSet(iIndex))
      return 0;
   char *pszRaw = apClRows[lRow]->getEntry(iIndex, __LINE__);
   if (!pszRaw)
      return 0;
   return (pszRaw[0] == 'i') ? 1 : 0;
}

// public: tell if row exists
bool Array::hasRow(int iRow) {
   if (ensureBase()) return 0;
   if (iRow < 0) {  perr("%s: illegal row index: %d on hasRow\n",pszClID,iRow); return 0; }
   return (iRow < nClRowsUsed) ? 1 : 0;
}

LongTable::LongTable() {
   nClArraySize = 0;
   nClArrayUsed = 0;
   pClArray     = 0;
}

LongTable::~LongTable() {
   resetEntries();
}

void LongTable::resetEntries() {
   nClArrayUsed = 0;
   if (pClArray)
      delete [] pClArray;
   pClArray = 0;
   nClArraySize = 0;
}

int LongTable::numberOfEntries() {
   return nClArrayUsed;
}

int LongTable::expand(int nSoMuch) {
   int *apTmp = new int[nClArraySize+nSoMuch];
   if (!apTmp) return 9;
   if (pClArray) {
      memcpy(apTmp, pClArray, nClArraySize*sizeof(int));
      delete [] pClArray;
   }
   pClArray = apTmp;
   nClArraySize += nSoMuch;
   return 0;
}

int LongTable::addEntry(int nValue, int nAtPos)
{
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }
   if (nAtPos != -1) {
      for (int i=nClArrayUsed; i>nAtPos; i--)
         pClArray[i] = pClArray[i-1];
      pClArray[nAtPos] = nValue;
      nClArrayUsed++;
   } else {
      pClArray[nClArrayUsed++] = nValue;
   }
   return 0;
}

int LongTable::updateEntry(int nValue, int nIndex) {
   if (nIndex >= nClArrayUsed)
      return 9+perr("wrong index for updateEntry: %d\n", nIndex);
   pClArray[nIndex] = nValue;
   return 0;
}

int LongTable::getEntry(int nIndex, int nTraceLine) {
   if (nIndex >= 0 && nIndex < nClArrayUsed)
      return pClArray[nIndex];
   perr("illegal LongTable index: %d %d tline %d\n", nIndex, nClArrayUsed, nTraceLine);
   return -1;
}

NumTable::NumTable() {
   nClArraySize = 0;
   nClArrayUsed = 0;
   pClArray     = 0;
}

NumTable::~NumTable() {
   resetEntries();
}

void NumTable::resetEntries() {
   nClArrayUsed = 0;
   if (pClArray)
      delete [] pClArray;
   pClArray = 0;
   nClArraySize = 0;
}

int NumTable::numberOfEntries() {
   return nClArrayUsed;
}

int NumTable::expand(int nSoMuch) {
   num *apTmp = new num[nClArraySize+nSoMuch];
   if (!apTmp) return 9;
   if (pClArray) {
      memcpy(apTmp, pClArray, nClArraySize*sizeof(num));
      delete [] pClArray;
   }
   pClArray = apTmp;
   nClArraySize += nSoMuch;
   return 0;
}

int NumTable::addEntry(num nValue, int nAtPos) {
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }
   if (nAtPos != -1) {
      for (int i=nClArrayUsed; i>nAtPos; i--)
         pClArray[i] = pClArray[i-1];
      pClArray[nAtPos] = nValue;
      nClArrayUsed++;
   } else {
      pClArray[nClArrayUsed++] = nValue;
   }
   return 0;
}

int NumTable::updateEntry(num nValue, int nAtPos) {
   if (nAtPos >= nClArrayUsed)
      return 9+perr("illegal index for updateEntry: %d\n", nAtPos);
   pClArray[nAtPos] = nValue;
   return 0;
}

num NumTable::getEntry(int nIndex, int nTraceLine) {
   if (nIndex >= 0 && nIndex < nClArrayUsed)
      return pClArray[nIndex];
   perr("illegal NumTable index: %d %d tline %d\n", nIndex, nClArrayUsed, nTraceLine);
   return -1;
}

// - - - coi caching object identifier

#define delstring(x) \
   if (x) {          \
      delete [] x;   \
      x = 0;         \
   }

bool Coi::bClDebug = 0;

extern bool endsWithPathChar(char *pszPath, bool bAcceptFWSlash);
extern uchar *getWebDemoData(int &isize);

Coi::Coi(char *pszName, char *pszRootDir)
{
   memset(this, 0, sizeof(*this));

   if (!pszName)
      { perr("int. #21161820"); return; }

   if (!strncmp(pszName, glblDotSlash, 2))
      pszName += 2;

   bool bIsNet = 0;

   #ifdef VFILEBASE
   bool strEnds(char *pszStr, char *pszPat);

   bool bexpdone = 0;

   if (   strBegins(pszName, "http://")
       || strBegins(pszName, "https://")
       || strBegins(pszName, "ftp://")
      )
   {
      // auto-expand http://thehost to http://thehost/
      bIsNet = 1;
      char *psz = strstr(pszName, "//");
      if (psz) {
         psz += 2;
         psz = strchr(psz, '/');
         if (!psz) {
            int nlen = strlen(pszName);
            pszClName = new char[nlen+4];
            strcpy(pszClName, pszName);
            strcat(pszClName, "/");
            bexpdone = 1;
         }
      }
   }

   if (!bexpdone)
   #endif // VFILEBASE

   if (!(pszClName = strdup(pszName))) {
      perr("out of memory: coi.ctr.1");
      return;
   }

   pszClRoot = pszRootDir ? strdup(pszRootDir) : 0;

   mtklog(("coi ctr %p name %s proot %p", this, pszName, pszClRoot));
}

#define delstring(x) \
   if (x) {          \
      delete [] x;   \
      x = 0;         \
   }

Coi::Coi(int iFromInfo)
{
   memset(this, 0, sizeof(*this));
}

Coi::~Coi( )
{
   mtklog(("coi dtr %p %s\n", this, pszClName));

   if (nClRefs > 0) {
      perr("url has %d refs open, cannot dtr: %s", nClRefs, pszClName);
      return; // avoid instant crash
   }

   if (pdata) {
      // missing cleanup tolerance:
      if (data().pfile) {
         perr("missing close on %s",name());
         fclose(data().pfile);
      }
      // cleanup heavy data:
      delete pdata;
   }

   delstring(pszClName);
   delstring(pszClUName);
   if (pwClName) { delete [] pwClName; pwClName = 0; }

   delstring(pszClRoot);
   delstring(pszClRef);
   delstring(pszClExtStr);

   memset(this, 0, sizeof(*this));
}

bool Coi::hasName( ) { return pszClName ? 1 : 0; }
bool Coi::hasBadName( ) { return bClBadName; }

Coi *Coi::copy( )
{
   Coi *pres = new Coi(name(), pszClRoot ? pszClRoot : 0);
   if (pszClRef) pres->setRef(pszClRef);
   if (pszClExtStr) pres->setExtStr(pszClExtStr);

   pres->nClStatus = nClStatus;
   pres->nClHave   = nClHave;
   pres->nClSize   = nClSize;
   pres->nClMTime  = nClMTime;
   pres->nClCTime  = nClCTime;
   pres->bClRead   = bClRead;
   pres->bClWrite  = bClWrite;
   pres->bClDir    = bClDir;
   pres->bClFSDir  = bClFSDir;
   pres->bClHidden = bClHidden;
   pres->bClLink   = bClLink;
   pres->bClBinary = bClBinary;
   pres->bClArc    = bClArc;
   pres->nClUCS    = nClUCS;
   pres->nClAttr   = nClAttr;
   pres->bClUniName = bClUniName;
   pres->crc       = crc;

   // mtklog(("coi.mtime: %u from copy",(uint)pres->nClMTime));

   return pres;
}

int Coi::incref(cchar *pTraceFrom)  {
   ++nClRefs;
   mtklog(("coi-ref-inc: cnt=%d from %s on %p %s", nClRefs, pTraceFrom, this, name()));
   return nClRefs;
}

int Coi::decref( )  {
   nClRefs--;
   mtklog(("coi-ref-dec: cnt=%d on %p %s", nClRefs, this, name()));
   if (nClRefs < 0)
      pwarn("ref.cnt underflow (%d): %s\n", name());
   return nClRefs;
}

int Coi::refcnt( )  {
   return nClRefs;
}

bool Coi::debug( ) { return cs.debug || bClDebug; }

void Coi::fillFrom(void *pfdatin) // SFKFindData ptr
{
   SFKFindData *pfdat = (SFKFindData *)pfdatin;

   bClDir    = (pfdat->attrib & 0x10) ? 1 : 0; // FILE_ATTRIBUTE_DIRECTORY

   bClHidden = (pfdat->attrib & 0x06) ? 1 : 0;

   #ifdef _WIN32
   // sfk176 identify NTFS links to support -nofo
   bClLink   = (pfdat->attrib & 0x400) ? 1 : 0; // FILE_ATTRIBUTE_REPARSE_POINT
   #else
   bClLink   = pfdat->islink ? 1 : 0;
   #endif

   nClMTime  = pfdat->time_write;
   nClCTime  = pfdat->time_create;
   nClSize   = pfdat->size;

   // mtklog(("coi.mtime: %u from fillFrom",(uint)nClMTime));

   // 0x01 of windows finddata.attrib is readonly
   bClWrite  = (pfdat->attrib & 0x01) ? 0 : 1;

   #ifndef _WIN32
   // linux specific and informal stuff
   rawmode   = pfdat->rawmode;
   rawtype   = pfdat->rawtype;
   rawnlnk   = pfdat->rawnlnk;
   if (pfdat->bhavenode) {
      nClINode  = pfdat->ninode;
      memcpy(&oClStDev, &pfdat->ostdev, sizeof(__dev_t));
      nClHave  |= COI_HAVE_NODE;
   }
   #endif

   // flag the status fields that have been copied
   nClHave  |= COI_HAVE_DIR;
   nClHave  |= COI_HAVE_HIDDEN;
   nClHave  |= COI_HAVE_LINK;
   nClHave  |= COI_HAVE_TIME;
   nClHave  |= COI_HAVE_SIZE;

   mtklog(("coi::fillfrom done dir=%d %s", bClDir, name()));

   // remember we don't need to readStat():
   nClStatus = 1; // meta infos now available
}

int Coi::status( ) {
   return (int)nClStatus;
}

#ifndef _WIN32
bool  Coi::haveNode( )   { return (nClHave & COI_HAVE_NODE) ? 1 : 0; }
num   Coi::getNode( )    { return nClINode; }
bool  Coi::haveFileID( ) { return haveNode(); }

const char *pGlblHexChars = "0123456789ABCDEF";

char *Coi::getFileID( )
{
   if (!haveNode()) return str("");

   // decode binary stdev and inode into a hex string
   uchar *pdst   = (uchar*)szClFileID;
   uchar *pmax   = pdst + sizeof(szClFileID)-4;

   uchar *psrc    = (uchar*)&oClStDev;
   int  nsrcsize = sizeof(oClStDev);
   int  isrc     = 0;

   if (pdst + nsrcsize * 2 >= pmax) return str("");
   for (isrc=0; isrc<nsrcsize; isrc++) {
      *pdst++ = pGlblHexChars[(*psrc >> 4) & 0x0F];
      *pdst++ = pGlblHexChars[(*psrc >> 0) & 0x0F];
      psrc++;
   }

   psrc = (uchar*)&nClINode;
   nsrcsize = sizeof(nClINode);

   if (pdst + nsrcsize * 2 >= pmax) return str("");
   for (isrc=0; isrc<nsrcsize; isrc++) {
      *pdst++ = pGlblHexChars[(*psrc >> 4) & 0x0F];
      *pdst++ = pGlblHexChars[(*psrc >> 0) & 0x0F];
      psrc++;
   }

   *pdst = '\0';

   if (cs.debug) {
      printf("fileid=\"%s\" len=%d for \"%s\"\n",szClFileID,(int)strlen(szClFileID),name());
   }

   return szClFileID;
}
#endif

CoiData::CoiData( ) {
   mtklog(("coidata ctr %p",this));
   memset(this, 0, sizeof(*this));
   #ifdef _WIN32
   otrav = -1;
   #endif
}

CoiData::~CoiData( )
{
   mtklog(("coidata dtr %p", this));

   #ifdef VFILEBASE

   if (pClFtp) {
      mtklog(("coidata dtr autoreleases ftp %p", pClFtp));
      releaseFtp();
      // do NOT delete, is managed by cache.
   }

   if (pClHeaders) {
      mtklog(("coidata dtr autoreleases headers %p", pClHeaders));
      delete pClHeaders;
   }

   if (pClHttp) {
      mtklog(("coidata dtr autoreleases http %p begin", pClHttp));
      releaseHttp();
      // do NOT delete, is managed by cache.
   }

   if (pelements) {
      mtklog(("coidata dtr autoreleases pelements %p", pelements));
      delete pelements;
   }

   if (pClOrgName) {
      mtklog(("coidata dtr autoreleases orgname %p", pClOrgName));
      delete [] pClOrgName;
   }

   #endif // VFILEBASE

   if (src.data) {
      mtklog(("coidata dtr autoreleases src.data %p", src.data));
      delete [] src.data;
   }

   if (rbuf.data) delete [] rbuf.data;

   if (prelsubname) delete [] prelsubname;


   if (bdiropen) perr("missing closeDir() after dir traversal, %p", this);

   if (pdirpat)  delete [] pdirpat;

   memset(this, 0, sizeof(*this));
   #ifdef _WIN32
   otrav = -1;
   #endif
}

cchar *officeExtList[] =
{
   // ms office
   ".docx",".dotx",".dotm",".docb",
   ".xlsx",".xlsm",".xltx",".xltm",
   ".pptx",".pptm",".potx",".potm",
   ".ppam",".ppsx",".ppsm",".sldx",".sldm",

   // ODF
   ".odt",".ods",".odp",".odg",".odc",".odf",".odi",".odm",
   ".ott",".ots",".otp",".otg",

   0
};

cchar *arcExtList[] =
{
   ".zip",".jar",".ear",".war",".aar",".xpi",
   ".jmod", // sfk1953

   // ms office sfk193
   ".docx",".dotx",".dotm",".docb",
   ".xlsx",".xlsm",".xltx",".xltm",
   ".pptx",".pptm",".potx",".potm",
   ".ppam",".ppsx",".ppsm",".sldx",".sldm",

   // ODF
   ".odt",".ods",".odp",".odg",".odc",".odf",".odi",".odm",
   ".ott",".ots",".otp",".otg",

   #ifdef VFILEBASE
   #ifndef NO_TAR_SUPPORT
   // CONVENTION:
   //   on ambiguous extensions like .tar <-> .tar.gz,
   //   always list the LONGEST FIRST.
   ".tar.gz", ".tar", ".tgz",
   ".tar.bz2", ".tbz2",
   ".gz", ".bz2",
   // NOTE: .tbz not supported, as it is not clearly bzip2
   #endif
   #endif // VFILEBASE

   // up to 30 user defined ZIP archive extensions are supported:
   0,0,0,0,0, 0,0,0,0,0,
   0,0,0,0,0, 0,0,0,0,0,
   0,0,0,0,0, 0,0,0,0,0,
   0,0,0,0,0  // EOD and some tolerance
};

class StartupEnvGrabber
{
public:
   StartupEnvGrabber();

   char szZipExtBuf[200];
};

StartupEnvGrabber glblEnvGrabber;

StartupEnvGrabber::StartupEnvGrabber()
{
   // set SFK_ZIP_EXT=".foo .bar .myext"
   char *psz1 = getenv("SFK_ZIP_EXT");
   if (psz1)
   {
      // find insertion index in arcExtList
      int iins = 0;
      int imax = (sizeof(arcExtList) / sizeof(char*)) - 3;
      for (; arcExtList[iins] && (iins<imax); iins++);

      // copy and split user extension list
      strcopy(szZipExtBuf, psz1);
      psz1 = szZipExtBuf;
      char *psz2 = 0;
      bool bterminal = 0;
      while (!bterminal && psz1 && *psz1)
      {
         psz1 = strchr(psz1, '.');
         if (!psz1) break;
         psz2 = psz1+1;
         while (*psz2 && isalnum(*psz2))
            psz2++;
         bterminal = *psz2 ? 0 : 1;
         *psz2++ = '\0';
         // still space left?
         if (iins >= imax-1) {
            fprintf(stderr, "Error: too many extensions in SFK_ZIP_EXT\n");
            break;
         }
         // add another extension
         arcExtList[iins++] = psz1;
         // step to next, if !bterminal
         psz1 = psz2;
      }
   }
}

bool endsWithExt(char *pname, char *pszextin)
{
   char *psz = strrchr(pname, '.');
   if (!psz)
      return 0;
 
   if (mystricmp(psz, pszextin))
      return 0;

   return 1;
}

bool endsWithArcExt(char *pname, int iTraceFrom)
{
   // find relevant end of input name:
   // filesys : e.g. .tar.bz2
   // http url: e.g. .tar.bz2?name=val&foo=bar

   // exclusive name end point (on terminator or '?'):
   char *pend = pname + strlen(pname);

   if (   strBegins(pname, "http://")
       || strBegins(pname, "https://")
      )
   {
      char *psz = strrchr(pname, '?');
      if (psz) pend = psz;
   }

   // netto length until end point
   int nnamlen = pend - pname;

   for (int i=0; arcExtList[i]; i++)
   {
      cchar *ppatstr = arcExtList[i];
      int  npatlen  = strlen(ppatstr);
      if (nnamlen < npatlen)
         continue;
      if (!mystrnicmp(pname + nnamlen - npatlen, ppatstr, npatlen)) {
         if (cs.debug)
            printf("arc.byext: %s [%d]\n", pname, iTraceFrom);
         return 1;
      }
   }

   if (cs.debug)
      printf("noarc.byext: %s [%d]\n", pname, iTraceFrom);

   return 0;
}

bool endsWithOfficeExt(char *pname, int iTraceFrom)
{
   // exclusive name end point (on terminator or '?'):
   char *pend = pname + strlen(pname);

   if (   strBegins(pname, "http://")
       || strBegins(pname, "https://")
      )
   {
      char *psz = strrchr(pname, '?');
      if (psz) pend = psz;
   }

   // netto length until end point
   int nnamlen = pend - pname;

   for (int i=0; officeExtList[i]; i++)
   {
      cchar *ppatstr = officeExtList[i];
      int  npatlen  = strlen(ppatstr);
      if (nnamlen < npatlen)
         continue;
      if (!mystrnicmp(pname + nnamlen - npatlen, ppatstr, npatlen)) {
         return 1;
      }
   }

   return 0;
}

// in case of file masks, "!.zip" shall NOT
// implicitely activate archive processing.
bool maskEndsWithArcExt(char *pname, int iTraceFrom)
{
   if (isNotChar(*pname))
      return 0;
 
   return endsWithArcExt(pname, iTraceFrom);
}

bool isArcFile(char *pname)
{
   if (endsWithArcExt(pname, 1))
      return 1;

   #ifdef SFKDEEPZIP
   if (cs.probefiles)
   {
      Coi ocoi(pname, 0);
      ocoi.probeFile();
      if (ocoi.isKnownArc())
         return 1;
   }
   #endif // SFKDEEPZIP

   return 0;
}

bool Coi::isDirOpen( ) {
   return data().bdiropen;
}

bool Coi::hasSize  ( ) {
   return (nClHave & COI_HAVE_SIZE) ? 1 : 0; // fix 1770
}

bool Coi::hasTime  ( ) {
   return (nClHave & COI_HAVE_TIME) ? 1 : 0; // fix 1770
}

void Coi::setSize  (num nSize) {
   nClSize  = nSize;
   nClHave |= COI_HAVE_SIZE;
}

void Coi::setTime(num nMTime, num nCTime)
{
   nClMTime = nMTime;
   nClCTime = nCTime;
   nClHave |= COI_HAVE_TIME;
   // mtklog(("coi.mtime: %u from setTime (%p, %s)",(uint)nClMTime,this,name()));
}

int Coi::remove( )
{
   #ifdef _WIN32
   if (vname()) {
      sfkname oname(name());
      return _wremove((const wchar_t *)oname.wname());
   }
   #endif

   return ::remove(name());
}

int Coi::closeAndRemove( )
{
   if (isFileOpen())
      close();
   return remove();
}

int Coi::setFileTime(num nMTime)
{
   #ifdef _WIN32

   #ifdef WINFULL
   if (vname())
   do
   {
      int irc = 0;

      sfkname oname(name());

      HANDLE hDst = CreateFileW(
         (const wchar_t *)oname.wname(),
         FILE_WRITE_ATTRIBUTES,
         0,    // share
         0,    // security
         OPEN_EXISTING,
         bClDir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL,
         0     // template file
         );
      if (hDst == INVALID_HANDLE_VALUE)
         return 9;

      FILETIME nDstMTime;
      FILETIME *pMTime=0;

      if (makeWinFileTime(nMTime, nDstMTime)) // coi.setFileTime
         irc = 10;
      else
      {
         pMTime = &nDstMTime;
         if (!SetFileTime(hDst, 0, 0, pMTime))
            irc = 11;
      }

      CloseHandle(hDst);

      return irc;
   }
   while (0);
   #endif

   #endif

   return mySetFileTime(name(), nMTime);
}

void Coi::setIsDir (bool bYesNo) {
   bClDir   = bYesNo;
   nClHave |= COI_HAVE_DIR;
}

num Coi::getSize( )
{
   if (nClHave & COI_HAVE_SIZE) return nClSize;
   if (!status()) readStat('s');
   return (status() >= 9) ? -1 : nClSize;
}

num Coi::getTime( )
{
   if (nClHave & COI_HAVE_TIME) {
      if (cs.usectime) return nClCTime;
      return nClMTime;
   }

   if (!status()) readStat('t');
   if (status() >= 9) return -1;

   if (cs.usectime) return nClCTime;
   return nClMTime;
}

uint Coi::getAttr( )
{
   if (nClHave & COI_HAVE_ATTR) return nClAttr;
   if (!status()) readStat('a');
   return (status() >= 9) ? 0 : nClAttr;
}

// RC >= 9: cannot set attributes
int Coi::writeAttr(uint nuxattr, bool bFullPreserve)
{
   int iSubRC = writeAttrRaw(name(), nuxattr, bFullPreserve, 0);

   if (!iSubRC)
      nClAttr = nuxattr;

   return iSubRC;
}

uint lastErrno()
{
   #ifdef _WIN32
   return GetLastError();
   #else
   return errno;
   #endif
}

int Coi::writeAttrRaw(char *pszFile, uint nuxattr, bool bFullPreserve, bool bVerbose)
{
   // valid attr are marked by this bit
   if (!(nuxattr & (1U << 31)))
      return 9; // invalid input

   #ifdef _WIN32

   uint nwinattr = GetFileAttributes(pszFile);
 
   if (nwinattr == INVALID_FILE_ATTRIBUTES)
      return 10;

   uint noldattr = nwinattr;
 
   // under windows, 'r' and 'x' flags are ignored.
   // only if all 'w' flags are cleared it is set readonly.

   if (   (nuxattr & (1U << 1)) // write.everyone
       || (nuxattr & (1U << 4)) // write.group
       || (nuxattr & (1U << 7)) // write.owner
      )
   {
      // disable readonly if set
      nwinattr &= (0xFFFFFFFFUL ^ FILE_ATTRIBUTE_READONLY);
   }
   else
   {
      // set readonly
      nwinattr |= FILE_ATTRIBUTE_READONLY;
   }

   if (noldattr != nwinattr) {
      if (!SetFileAttributes(pszFile, nwinattr)) {
         if (bVerbose)
            pwarn("cannot set file attributes (%d): %s\n", lastErrno(), pszFile);
         return 11;
      }
   }

   return 0;

   #else

   struct stat64 ostat;
   if (stat64(pszFile, &ostat))
      return 10;

   static bool bGotUMask = 0;
   static uint nSysUMask = 0;
   if (!bGotUMask)
   {
      bGotUMask = 1;
      // to read umask, it must be changed temporarily
      nSysUMask = umask(0);
      // change back immediately, as we now have the value
      umask(nSysUMask);
   }

   if (!bFullPreserve)
   {
      // apply umask to input attributes.
      // bits set in umask must be deleted in attributes.
      uint uiReverse = (0xFFFFFFFFUL ^ nSysUMask);
      nuxattr &= uiReverse;
   }

   uint nmode = ostat.st_mode;
   uint noldmode = nmode;

   // allow only changing of the lower 12 bits.
   // also filters out sfk specific bits.
   nmode &= 0xFFFFF000UL;
   nmode |= (nuxattr & 0xFFFUL);

   if (noldmode != nmode)
      if (chmod(pszFile, nmode))
         return 11;

   return 0;

   #endif
}

// RC >= 9: cannot make this writeable
int Coi::forceWriteable(char *pszFile)
{
   #ifdef _WIN32

   uint nwinattr = GetFileAttributes(pszFile);
 
   if (nwinattr == INVALID_FILE_ATTRIBUTES)
      return 1; // nothing to do, does not exist

   if (!(nwinattr & FILE_ATTRIBUTE_READONLY))
      return 2; // nothing to do, is writeable

   nwinattr &= (0xFFFFFFFFUL ^ FILE_ATTRIBUTE_READONLY);

   if (!SetFileAttributes(pszFile, nwinattr))
      return 9; // cannot change

   #else

   struct stat64 ostat;
   if (stat64(pszFile, &ostat))
      return 1; // nothing to do, does not exist

   uint nmode = ostat.st_mode;
 
   if (nmode & (1UL << 7))
      return 2; // nothing to do, is writeable

   nmode |= (1UL << 7);

   if (chmod(pszFile, nmode))
      return 9; // cannot change

   #endif

   return 0; // OK changed
}

int Coi::setExtStr(char *psz) {
   delstring(pszClExtStr);
   if (psz)
      if (!(pszClExtStr = strdup(psz)))
         return 9;
   return 0;
}

char *Coi::getExtStr( ) {
   return pszClExtStr;
}

int Coi::readStat(char cFromInfo)
{__
   // NOTE: readStat cannot fetch "hidden" and "link" file info.
   //       such info seems to be available only via findFirstFile().

   // if filesystem meta infos already read,
   // or any error already occured, do nothing:
   if (nClStatus > 0)
      return nClStatus;

   #ifdef SFKPACK
   if (isOfficeSubEntry())
   {
      // defensive information gathering:
      // if ANY infos exist, assume that the vfile meta info
      // have already been fetched in a different way,
      // so avoid expensive retrieval operations.
      if (nClHave) {
         nClStatus = 1;
         return 0;
      }

      #if defined(VFILEBASE)
      // no expensive head access.
      if (isHttp()) {
         #ifdef SFKINT
         pwarn("stat: wrong http call sequence (%c).\n", cFromInfo);
         #endif
         nClStatus = 9;
         return 9;
      }
      #endif

      // in any case, block nativeFS access on virtuals:
      nClStatus = 1; // "sorry, we know nothing."
      return 0;
   }
   #endif // SFKPACK

   #ifdef VFILEBASE
   // virtual files MUST be blocked from native FS access
   if (isNet() || isZipSubEntry())
   {
      // defensive information gathering:
      // if ANY infos exist, assume that the vfile meta info
      // have already been fetched in a different way,
      // so avoid expensive retrieval operations.
      if (nClHave) {
         nClStatus = 1;
         return 0;
      }

      // no expensive head access.
      if (isHttp()) {
         #ifdef SFKINT
         pwarn("stat: wrong http call sequence (%c).\n", cFromInfo);
         #endif
         nClStatus = 9;
         return 9;
      }

      // in any case, block nativeFS access on virtuals:
      nClStatus = 1; // "sorry, we know nothing."
      return 0;
   }
   #endif // VFILEBASE

   sfkstat_t ostat;

   int bIsDir    = 0;
   int bCanRead  = 1;
   int bCanWrite = 1;
   num  nFileTime = 0;
   num  nFileSize = 0;
   num aExtTimes[2];
   memset(aExtTimes, 0, sizeof(aExtTimes));

   #ifdef SFK_W64
   if (vname())
   {
      sfkname oname(name());

      struct __stat64 buf;
      if (_wstat64((const wchar_t *)oname.wname(), &buf)) {
         nClStatus = 9;
         return 9;
      }
      bIsDir = (buf.st_mode & _S_IFDIR ) ? 1 : 0;
      bCanRead     = (buf.st_mode & _S_IREAD ) ? 1 : 0;
      bCanWrite    = (buf.st_mode & _S_IWRITE) ? 1 : 0;
      nFileTime    = buf.st_mtime;
      nFileSize    = buf.st_size;
      aExtTimes[0] = buf.st_ctime;
      aExtTimes[1] = buf.st_atime;
      mclear(ostat);
      ostat.st_mode= buf.st_mode;
   }
   else
   #endif
   if (getFileStat(name(), bIsDir, bCanRead, bCanWrite,
      nFileTime, nFileSize, aExtTimes,
      &ostat, sizeof(ostat)
      ))
   {
      mtklog(("coi.stat failed: %s have=%u from=%c", name(), nClHave, cFromInfo));
      nClStatus = 9;
      return 9;
   }

   nClStatus = 1; // read metadata successfully

   nClSize  =       nFileSize;   nClHave |= COI_HAVE_SIZE;
   nClMTime =       nFileTime;   nClHave |= COI_HAVE_TIME;
   nClCTime =    aExtTimes[0];
   bClRead  = (bool)bCanRead;    nClHave |= COI_HAVE_READ;
   bClWrite = (bool)bCanWrite;   nClHave |= COI_HAVE_WRITE;
   bClDir   = (bool)bIsDir;      nClHave |= COI_HAVE_DIR;

   nClAttr  = ostat.st_mode;     nClHave |= COI_HAVE_ATTR;

   // add sfk flags
   nClAttr |= (1U << 31);  // attribs are valid
   #ifndef _WIN32
   nClAttr |= (1U << 30);  // source is linux
   #endif
 
   // mtklog(("coi.mtime: %u from readStat",(uint)nClMTime));
   mtklog(("coi.stat r=%d w=%d dir=%d %s", bClRead, bClWrite, bClDir, name()));

   return 0;
}

bool Coi::isWriteable( )
{
   #ifdef SFKPACK
   if (isOffice(101)) return 0;
   #endif // SFKPACK
 
   #ifdef VFILEBASE
   // any virtual file?
   if (isNet()) return 0;
   if (isZipSubEntry() > 0) return 0;
   if (isTravelZip(101)) return 0;
   #endif // VFILEBASE

   mtklog(("coi::iswriteable.1 %d %d %s",bClWrite,nClStatus,name()));

   // only physical files are writeable, so far.
   if (!nClStatus) readStat('w');
   if (bClDir) return 0;

   mtklog(("coi::iswriteable.2 %d %d %s",bClWrite,nClStatus,name()));

   return bClWrite;
}

char *Coi::relName( )
{
   char *pszabs = name();


   // native file:
   char *prel = strrchr(pszabs, glblPathChar);
   if (prel) return prel+1;

   #ifdef _WIN32
   // c:thefile.txt
   prel = strrchr(pszabs, ':');
   if (prel) return prel+1;
   #endif

   return pszabs;
}

char *Coi::rootRelName( )
{
   char *relName(char *pszRoot, char *pszAbs);
   if (pszClRoot)
   {
      mtklog(("coi.rootrelname: %s %.10s %.10s", relName(pszClRoot, pszClName), pszClRoot, pszClName));
      if (debug()) printf("coi.rootrelname: %s\n", relName(pszClRoot, pszClName));
      char *prel = relName(pszClRoot, pszClName);
      // special case: theroot.zip\\thesub.txt
      // then now holding: "\thesub.txt"
      if (*prel == glblPathChar) prel++;
      return prel;
   }
   if (debug()) printf("coi.rrname.noroot: %s\n", pszClName);
   return pszClName;
}

bool useOfficeBaseNames( )
{
   #ifdef SFKOFFICE
   if (!cs.office)
      return 0;
   return 1; // office basenames always
   #endif // SFKOFFICE
   return 0;
}

#ifdef SFKPACK
// from    "thefoo.xlsx\\bar.txt"
// returns "bar.txt", or NULL if no zip subname
char *Coi::officeSubName( )
{
   char *pname = name();

   char *purl = strstr(pname, "://");
   if (purl)
         pname = purl + 3;

   char *pxsla = strstr(pname, glblDubPathStr);
   if (!pxsla) return 0; // no pattern hit

   // do NOT accept foo.xlsx\\bar.xlsx\\goo.xml for office.
   // this must be handled by xe zip reading.
   if (strstr(pxsla+2, glblDubPathStr)) return 0;

   long nnamlen = pxsla - pname;

   // accept only office extensions.
   // generic zip is reserved for xe.
   for (long i=0; officeExtList[i]; i++)
   {
      cchar *ppatstr = officeExtList[i];
      long  npatlen  = strlen(ppatstr);
      if (nnamlen < npatlen)
         continue;
      if (striBegins(pxsla - npatlen, ppatstr))
         return pxsla+1;
   }

   return 0;
}
#endif // SFKPACK

void stripOfficeName(char *pname)
{
   char *pxsla = mystrrstr(pname, glblDubPathStr);
   if (!pxsla) return;

   *pxsla = '\0';
}

#ifdef SFKPACK
void Coi::stripOfficeName()
{
   if (!pszClName) return;

   char *pxsla = mystrrstr(pszClName, glblDubPathStr);
   if (!pxsla) return;

   *pxsla = '\0';

   // force re-read of file stats
   nClStatus = 0;
   nClHave = 0;
}

int Coi::isOfficeSubEntry()
{
   #ifdef SFKOFFICE
   char *pzip = officeSubName();
   if (!pzip)    return 0;
   if (!pzip[0]) return 0; // from "thefile.zip//"

   long nlen = strlen(pzip);
   if (nlen > 0 && (pzip[nlen-1] == glblPathChar || pzip[nlen-1] == glblWrongPChar))
      return 2;   // thefile.zip//subdir/

   return 1;   // thefile.zip//subfile.dat
   #endif // SFKOFFICE

   return 0;
}

bool Coi::isOffice(int iTraceFrom, bool bIgnoreOfficeMode)
{
   #ifdef SFKOFFICE

   if (!bIgnoreOfficeMode && !cs.office) return 0;

   if (bClFSDir) return 0; // native file system dir
   if (isOfficeSubEntry()) return 0;
   #ifdef VFILEBASE
   if (isNet()) return 0;
   #endif // VFILEBASE

   #ifdef SFKPACK
   if (endsWithOfficeExt(name(), iTraceFrom))
      return 1;
   #endif // SFKPACK

   #endif // SFKOFFICE

   return 0;
}
#endif // SFKPACK

bool Coi::isAnyDir(int ilevel)
{
   if (bGlblSyntaxTest) {
      // if just simulating, check how the name looks
      if (!strcmp(name(), ".")) return 1;
      return strstr(name(), "dir") ? 1 : 0;
   }

   int n = 0;

   #ifdef SFKDEEPZIP
   if (cs.probefiles)
      probeFile(); // to read isKnownArc() status
   #endif // SFKDEEPZIP

   #ifdef SFKPACK
   if (isOffice(100)) {
      bClDir = 1;
      nClHave |= COI_HAVE_DIR;
      return bClDir;
   }
   #endif // SFKPACK

   #ifdef VFILEBASE
   if (isTravelZip(103)) {
      bClDir = 1;
      mtklog(("%d = isanydir(%s) by trvzip", bClDir, name()));
      nClHave |= COI_HAVE_DIR;
      return bClDir;
   }
   if ((n = isZipSubEntry()))
   {
      switch (n) {
         case 1: bClDir = 0; break; // file within zip
         case 2: bClDir = 1; break; // dir within zip (but not traversable)
      }
      mtklog(("%d = isanydir(%s) by zipsub (%d)", bClDir, name(), n));
      nClHave |= COI_HAVE_DIR;
      return bClDir;
   }
   #endif // VFILEBASE

   if (nClHave & COI_HAVE_DIR) {
      mtklog(("%d = isanydir(%s) as stored", bClDir, name()));
      return bClDir;
   }

   // dir status not yet determined
   bClDir = 0;

   #ifdef VFILEBASE
   if (isHttp()) {
      bClDir = rawIsHttpDir(ilevel);
      mtklog(("%d = isanydir(%s) by httpdir", bClDir, name()));
   }
   else
   if (isFtp()) {
      bClDir = rawIsFtpDir();
      mtklog(("%d = isanydir(%s) by ftpdir", bClDir, name()));
   }
   else
   #endif // VFILEBASE
   {
      bClDir = rawIsDir();
      mtklog(("%d = isanydir(%s) by fs", bClDir, name()));
   }

   // avoid redundant high-effort reads of dir info
   nClHave |= COI_HAVE_DIR;

   return bClDir;
}  // Coi::isAnyDir

bool Coi::isTravelDir(bool bTreatOfficeAsFile)
{
   if (bGlblSyntaxTest) {
      // if just simulating, check how the name looks
      if (!strcmp(name(), ".")) return 1;
      return strstr(name(), "dir") ? 1 : 0;
   }

   // make sure raw status is determined
   bool brawdir = isAnyDir();

   #ifdef SFKPACK
   if (isOffice(102)) {
      if (bTreatOfficeAsFile)
         brawdir = 0;
      else
         brawdir = 1; // per definitionem
      return brawdir;
   }
   #endif // SFKPACK

   #ifdef VFILEBASE
   if (isTravelZip(104))   brawdir = 1;   // per definitionem
   else
   if (isZipSubEntry())    brawdir = 0;   // travel done otherwise
   #endif // VFILEBASE

   mtklog(("%d = istravdir(%s)", brawdir, name()));

   return brawdir;
}

bool Coi::rawIsDir( ) {
   if (!nClStatus)
      readStat('d');
   return bClDir;
}

// used so far only for http redirects
int Coi::setName(char *psz, char *pszOptRoot)
{
   bClBadName = 0;

   #ifdef VFILEBASE
   if (pszClName && hasData() && !data().pClOrgName)
   {
      // do not delete first name,
      // but refunction it as the org name.
      data().pClOrgName = pszClName;
      pszClName = 0;
   }
   #endif // VFILEBASE

   if (pszClName)  { delete [] pszClName;  pszClName = 0; }
   if (pszClUName) { delete [] pszClUName; pszClUName = 0; }
   if (pwClName)   { delete [] pwClName;   pwClName = 0; }

   pszClName = strdup(psz);

   if (pszOptRoot) {
      if (pszClRoot) delete [] pszClRoot;
      pszClRoot = strdup(pszOptRoot);
   }

   return 0;
}

static ushort aEmptyWideName[2] = {0, 0};

char *Coi::name( )
{
   return pszClName ? pszClName : str("");
}

#ifdef VFILEBASE
char  *Coi::orgName( )
{
   if (hasData() && data().pClOrgName)
      return data().pClOrgName;
   return name();
}

bool Coi::wasRedirected( )
{
   if (hasData() && data().pClOrgName)
      return 1;
   return 0;
}
#endif // VFILEBASE

char  *Coi::root(bool braw) {
   if (braw) return pszClRoot;
   return pszClRoot ? pszClRoot : (char*)"";
}

char  *Coi::ref (bool braw) {
   if (braw) return pszClRef;
   return pszClRef ? pszClRef : (char*)"";
}

int  Coi::setRef(char *pszName) {
   delstring(pszClRef);
   if (pszName)
      if (!(pszClRef = strdup(pszName)))
         return 9;
   return 0;
}

CoiData &Coi::data( )
{
   if (!pdata)
      pdata = new CoiData();
   return *pdata;
}

bool Coi::hasData( )    { return pdata ? 1 : 0; }

#ifdef VFILEBASE
bool Coi::isCached( )   { return bClInCache; }
bool Coi::hasContent( ) { return (pdata && pdata->src.data) ? 1 : 0; }
#endif // VFILEBASE

bool Coi::isFileOpen( ) {
   // the open flag itself is managed via pdata
   if (!pdata) return 0;
   return data().bfileopen;
}

void Coi::setContent(uchar *pdata, num nsize, num ntime)
{__
   mtklog(("coi.setcontent size=%d time=%u (%p, %s)",(int)nsize,(uint)ntime,this,name()));

   num nOldSize = data().src.size;
   num nNewSize = nsize;

   if (data().src.data) {
      delete [] data().src.data;
      mclear(data().src);
   }

   if (pdata) {
 
      data().src.data = pdata;
      data().src.size = nsize;
      data().src.time = ntime;

      // fix: 173: missing meta data update after setContent
      setSize(nsize);
      setTime(ntime, ntime);
   }

   #ifdef VFILEBASE
   // cache checks itself if coi is really cached
   glblVCache.tellByteSizeChange(this, nOldSize, nNewSize);
   #endif // VFILEBASE
}

int Coi::releaseContent( )
{__
   setContent(0,0); // free existing

   if (data().rbuf.data) {
      mtklog(("coi releases rbuf: %s",name()));
      delete [] data().rbuf.data;
      mclear(data().rbuf);
   }

   return 0;
}

bool Coi::existsFile(bool bOrDir, int *pIsDir)
{
   char *pszName = name();

   #ifdef SFKPACK
   if (isOfficeSubEntry()) return 1; // assume it exists
   #endif // SFKPACK

   #ifdef VFILEBASE
   if (isNet())         return 1; // assume it exists
   if (isZipSubEntry()) return 1; // assume it exists
   // .zip file: fall through
   #endif // VFILEBASE

   #ifdef _WIN32

   DWORD nAttrib = 0;
   #ifdef SFK_W64
   if (vname())
   {
      sfkname oname(name());
      nAttrib = GetFileAttributesW((wchar_t *)oname.wname());
   }
   else
   #endif
      nAttrib = GetFileAttributes(pszName);
   if (nAttrib == 0xFFFFFFFF) // "INVALID_FILE_ATTRIBUTES"
      return 0;
   if (nAttrib & FILE_ATTRIBUTE_DIRECTORY) {
      if (!bOrDir)
         return 0; // is a dir, not a file
      if (pIsDir) *pIsDir=1;
   }

   #else

   // linux
   struct stat64 buf;
   if (stat64(pszName, &buf))
      return 0;
   if ((buf.st_mode & _S_IFDIR)) {
      if (!bOrDir)
         return 0; // is a dir, not a file
      if (pIsDir) *pIsDir=1;
   }

   #endif

   // we can get the attribs, and it's not a dir,
   // so expect that it is a file.
   return 1;
}

int Coi::getOpenElapsedTime( )
{
   if (!pdata)
      return 0;
 
   if (!data().nopentime)
      return 0;
 
   return (int)(getCurrentTime() - data().nopentime);
}

int Coi::open(cchar *pmode)
{__
   mtklog(("coi-open %p %s %s",this,name(),pmode));

   // reset per-file statistics
   resetIOStatus();

   #ifdef SFKPACK
   if (isOfficeSubEntry())
      if (loadOfficeSubFile("opn"))
         return 9;
   #endif // SFKPACK

   #ifdef VFILEBASE
   // in case of vfiles, make sure required inputs
   // are loaded, e.g. http://foo.zip//bar.zip//file.txt
   // RECURSION BLOCK: for now, we call provideInput
   // ONLY for zip sub entries. netFiles are expected
   // to be cached otherwise, but not through pinput.
   if (isZipSubEntry())
      if (provideInput("opn"))
         return 9;
   #endif // VFILEBASE

   // (re)init in case that readLine is used:
   if (data().rbuf.data) delete data().rbuf.data;
   memset(&data().rbuf, 0, sizeof(data().rbuf));

   // supported modes: "rb","r+b","wb"
   data().bwrite     = 0;
   data().szmode[0]  = '\0';
   data().ntotalread = 0;
   #ifdef VFILEBASE
   data().bstopread  = 0;
   #endif // VFILEBASE
   data().banyread   = 0;
   data().nopentime  = getCurrentTime();

   if (!strcmp(pmode, "rb"))
      { } // pure read is always ok
   else
   if (!strcmp(pmode, "r")) {
      // read of text: open as binary
      pmode = "rb";
      // but stop as soon as binary is detected.
      data().bstoprdbin = 1;
   }
   else
   if (   !strcmp(pmode, "r+b") || !strcmp(pmode, "wb")
       || !strcmp(pmode, "w") || !strcmp(pmode, "a") // +tofile
      )
   {
      #ifdef VFILEBASE
      if (isVirtual(1)) // WITH root zips
         return 9+perr("write not supported: %s", name());
      #endif // VFILEBASE
      // TODO: not sure if this protects utf16 files in every case,
      //       so far it requires an isBinary() check before.
      if (nClUCS)
         return 9+perr("write to utf-16 file not supported: %s", name());
      // read+write ok
      data().bwrite = 1;
   }
   else
      return 9+perr("unsupported mode \"%s\": %s", pmode, name());

   // remember I/O mode
   strcopy(data().szmode, pmode);

   // take cached data?
   if (data().src.data)
   {
      if (data().bwrite) {
         // re-write of a file: reset cache
         mtklog(("coi-open for write: reset cache"));
         setContent(0,0);
      } else {
         // read: simply reset read index
         mtklog(("coi-open uses CACHED DATA: bytes=%d time=%u (%p)",(int)data().src.size, (uint)nClMTime, this));
         data().src.index = 0;
         return 0;
      }
   }

   #ifdef VFILEBASE
   // this can be reached in READ mode only
   // http://thehost/thefile.txt
   if (isHttp()) return rawOpenHttpSubFile(pmode);
   if (isFtp())  return rawOpenFtpSubFile(pmode);
   #endif // VFILEBASE


   // native file I/O: thefile.dat
   #ifdef WINFULL
   if (vname())
   {
      sfkname omode(pmode);
      sfkname oname(name());
      data().pfile = _wfopen((const wchar_t *)oname.wname(), (const wchar_t *)omode.wname());
      // if (cs.debug)
      //   printf("wfopen: %s / %s / %p\n", name(), dataAsHex((uchar*)oname.wname(),wcslen((wchar_t*)oname.wname())*2), data().pfile);
   }
   else
   #endif
   {
      data().pfile = fopen(name(), pmode);
   }
   if (data().pfile) {
      data().bfileopen = 1;
      return 0;
   }
   return 9;
}

int Coi::renameto(char *pszDst)
{
   #ifdef _WIN32
   if (vname())
   {
      sfkname osrc(name());
      sfkname odst(pszDst);
      return _wrename((const wchar_t *)osrc.wname(), (const wchar_t *)odst.wname());
   }
   else
   #endif
   {
      return rename(name(), pszDst);
   }
}

cchar *Coi::lasterr( ) {
   if (!hasData()) return "";
   return data().szlasterr;
}

void Coi::setBinaryFile(bool bYesNo) {
   bClBinary = bYesNo;
   nClHave  |= COI_HAVE_BINARY;
}

bool isTextChar(uchar uc) {
   if (uc >= 0x1C) return 1;
   switch (uc) {
   // case 0x04: return 1; // EOT
   // case 0x07: return 1; // BEL
      case 0x08: return 1; // BS
      case 0x09: return 1; // TAB
      case 0x0A: return 1; // LF
      case 0x0C: return 1; // FF
      case 0x0D: return 1; // CR
      case 0x1A: return 1; // EOF
      case 0x1B: return 1; // ESC
   }
   return 0;
}

uchar Coi::isUTF16( ) { return nClUCS; }

bool Coi::isSnapFile( ) { return bClSnap; }

int Coi::iBinaryCheckSize = 4096;

void Coi::probeFile( ) { isBinaryFile(); }

bool Coi::isBinaryFile( )
{__ _p("sf.isbin")

   // if binary status was alread set, return it:
   if (nClHave & COI_HAVE_BINARY)
      return bClBinary;

   #ifdef VFILEBASE
   if (isHttp()) {
      // missing preloadFromWeb
      #ifdef SFKINT
      pwarn("isbinary: wrong http call sequence.\n");
      #endif
      return 1; // safety
   }
   #endif // VFILEBASE

   #ifdef SFKPACK
   if (isOffice(109)) {
      // so far we use only plaintext from office
      bClBinary = 0;
      nClHave  |= COI_HAVE_BINARY;
      return 0;
   }
   #endif // SFKPACK

   if (open("rb"))
      return 0;

   mtklog(("coi isbinary %p", this));
 
   // assure read buffer on demand
   if (!data().rbuf.data) {
      memset(&data().rbuf, 0, sizeof(data().rbuf));
      data().rbuf.data = new uchar[MY_GETBUF_MAX+100];
   }

   bool brc = 0;
 
   do
   {
      int nCheckLen = MY_GETBUF_MAX - 10;
 
      if (nCheckLen > iBinaryCheckSize)
         nCheckLen = iBinaryCheckSize;
 
      int nRead = readRaw(data().rbuf.data, nCheckLen);
 
      // no checks on empty files
      if (nRead <= 0)
         break;
 
      // always assure zero-term'ed memory
      if (nRead <= nCheckLen)
         data().rbuf.data[nRead] = '\0'; // safety
 
      // if (cs.wchardec)
      {
         // ucs-2 heuristic detection
         uint blehead=0,bbehead=0;
         uchar *pdat = data().rbuf.data;
 
         if (nRead>=2 && pdat[0]==0xFF && pdat[1]==0xFE) blehead=1;
         if (nRead>=2 && pdat[0]==0xFE && pdat[1]==0xFF) bbehead=1;
         if (blehead || bbehead) {
            // scan first part of text for double-byte chars.
            // accept only very low code points.
            int nWChars = (nRead - 2) / 2;
            if (nWChars > 10) nWChars = 10;
            int iwc=0;
            for (iwc=0; iwc<nWChars; iwc++) {
               uchar nlo = pdat[2+iwc*2+bbehead];
               uchar nhi = pdat[2+iwc*2+blehead];
               if (!isTextChar(nlo) || isTextChar(nhi))
                  break;
            }
            if (iwc >= nWChars) {
               // count detected ucs-2 files
               gs.utf16found++;
               mtklog(("utf16%s%s, dec=%d: %s\n",blehead?"le":"",bbehead?"be":"",(int)cs.wchardec,name()));
               // but are we allowed to decode?
               if (cs.wchardec) {
                  if (blehead) nClUCS = (uchar)0xFE;
                  if (bbehead) nClUCS = (uchar)0xEF;
                  if (cs.verbose)
                     printf("utf16%s%s: %s\n",blehead?"le":"",bbehead?"be":"",name());
               }
            }
         }
      }
 
 
      if (!brc && !nClUCS) {
         // binary data heuristic detection
         // for (int i=0; i<nRead; i++)
         //    if (data().rbuf.data[i] == 0x00)
         //       { brc=1; break; }
         if (memchr(data().rbuf.data, 0, nRead))
            brc=1;
      }
 
      // sfk snapfile detection
      {
         const char *ppat = ":snapfile sfk,";
         int npatlen = strlen(ppat);
         if ((nRead > npatlen) && strBegins((char*)data().rbuf.data, (char*)ppat))
            bClSnap = 1;

         ppat = ":cluster sfk,";
         npatlen = strlen(ppat);
         if ((nRead > npatlen) && strBegins((char*)data().rbuf.data, (char*)ppat))
            bClSnap = 1;
      }

      #ifdef SFKDEEPZIP
      {
         uchar *p = data().rbuf.data;
         if (nRead >= 24 && p[0]==0x50 && p[1]==0x4B
                         && p[2]==0x03 && p[3]==0x04)
         {
            setArc(1);
         }
      }
      #endif // SFKDEEPZIP
   }
   while (0);
 
   // currently, this frees the rbuf.data!
   close();

   bClBinary = brc;
   nClHave  |= COI_HAVE_BINARY;

   mtklog(("%d = coi::isbinary %s",brc,name()));

   return brc;
}

// TODO: rework rc handling in case of error
int Coi::readLine(char *pszOutBuf, int nOutBufLen)
{_p("sf.readln")

   if (!data().rbuf.data) {
      memset(&data().rbuf, 0, sizeof(data().rbuf));
      data().rbuf.data = new uchar[MY_GETBUF_MAX+100];
   }

   if (data().rbuf.getsize  < 0 || data().rbuf.getsize  > MY_GETBUF_MAX) return 0+perr("int. #62 %d %d\n",(data().rbuf.getsize < 0),(data().rbuf.getsize > MY_GETBUF_MAX));
   if (data().rbuf.getindex < 0 || data().rbuf.getindex > MY_GETBUF_MAX) return 0+perr("int. #63 %d %d\n",(data().rbuf.getindex < 0),(data().rbuf.getindex > MY_GETBUF_MAX));
   if (data().rbuf.geteod > 1) return 0+perr("int. #65\n");

   // if remaining data is less than halve of buffer, read next block
   if (!data().rbuf.geteod && ( data().rbuf.getsize < (MY_GETBUF_MAX/2)-100 ))
   {_p("sf.readl1")
 
      // move remaining cache data to front of buffer
      int nindex  = data().rbuf.getindex;
      int nremain = data().rbuf.getsize;
      if (nindex + nremain < 0) return 0+perr("int. #60\n");
      if (nindex + nremain > MY_GETBUF_MAX) return 0+perr("int. #61\n");
      if (nremain > (MY_GETBUF_MAX/2)) return 0+perr("int. #62\n");
      if (nremain > 0)
      {
         // FIX: 1722: read from zip with sfk x64 produced wrong data
         //      due to memcpy on overlapping buffer
         memmove(data().rbuf.data, &data().rbuf.data[nindex], nremain);
      }

      // now remaining data starts from front of buffer,
      // filling less than the first halve of the buffer.
      data().rbuf.getindex = 0;

      // re-fill remaining buffer space
      int nBufFree = MY_GETBUF_MAX - data().rbuf.getsize;
      uchar *pRead  = data().rbuf.data + data().rbuf.getsize;

      int nRead = 0;
      if ((nRead = read(pRead, nBufFree)) <= 0)
         data().rbuf.geteod = 1;
      else
         data().rbuf.getsize += nRead;
   }

   // anything remaining?
   if (data().rbuf.getsize == 0) {
      data().rbuf.geteod = 2;
      return 0;
   }

   // copy next line from copy index
   uchar *psrc     = data().rbuf.data + data().rbuf.getindex;
   uchar *psrcmax  = psrc + data().rbuf.getsize;
   if (psrcmax > data().rbuf.data + MY_GETBUF_MAX) return 0+perr("int. #63");

   uchar *pdst     = (uchar*)pszOutBuf;
   uchar *pdstmax  = pdst + nOutBufLen - 10;

   bool bBinary    = 0;
   int  nSrcBytes  = 0;
   int  nDstBytes  = 0;

   {_p("sf.readl2")
 
      for (; psrc < psrcmax && pdst < pdstmax;)
      {
         uchar c1 = *psrc++;
         nSrcBytes++;
 
         if (c1 == 0x00 || c1 == 0x1A) {
            if (!c1)
               bBinary = 1;
            c1 = (uchar)'.';
         }
         else
         if (c1 == (uchar)'\r')
            continue;
 
         *pdst++ = (char)c1;
         nDstBytes++;
 
         if (c1 == (uchar)'\n')
            break;
      }
      *pdst = '\0';
 
   }
 
   data().rbuf.getindex += nSrcBytes; // copy next line from there
   data().rbuf.getsize  -= nSrcBytes; // reduce remaining bytes in buf
   data().rbuf.getpos   += nSrcBytes; // absolute source position in file

   if (bBinary && data().bstoprdbin)
      return 0; // "EOD", force end of reading

   return nDstBytes;
}

int Coi::writeLine(char *psz)
{
   int iLen   = strlen(psz);
   int iChars = (int)write((uchar*)psz, strlen(psz));
   if (iChars != iLen)
      return iChars;

   uchar abLF[10];
   abLF[0] = '\n';
   abLF[1] = '\0';

   // depending on mode used with open
   // this writes LF or CRLF
   int iEOL = (int)write(abLF, 1);
   if (iEOL < 1)
      return 0;

   return iChars+iEOL;
}

size_t Coi::read(void *pbufin, size_t nBufSize)
{__ _p("sf.read")

   if (nClUCS && (nBufSize & 0x1UL)) {
      // force even buffer size on ucs-2
      nBufSize ^= (size_t)0x1UL;
      if (!nBufSize)
         return 0+perr("cannot read ucs-2, buffer too small: %s", name());
   }

   bool bfirst = data().banyread ? 0 : 1;

   size_t nraw = readRaw(pbufin, nBufSize);
   size_t nres = nraw; // resulting size

   data().banyread = 1;

   if (nClUCS)
   {
      // transparent ucs-2 decoding.
      // this will return less bytes than nBufSize!
      if (nBufSize & 1) // shouldn't happen due to above alignment
         return 0+perr("cannot read ucs-2, uneven buffer size: %s", name());

      uchar *psrc    = (uchar*)pbufin;
      uchar *psrcmax = (uchar*)pbufin + nraw;
      uchar *pdst    = (uchar*)pbufin;
      uchar *pdstbeg = pdst;
      uchar *pdstmax = (uchar*)pbufin + nBufSize;

      // skip byte order mark?
      if (nClUCS == 0xFE && psrc[0] == 0xFF && psrc[1] == 0xFE)
         psrc += 2;
      else
      if (nClUCS == 0xEF && psrc[0] == 0xFE && psrc[1] == 0xFF)
         psrc += 2;

      // copy loop with zero truncation
      uint nreloff = (nClUCS == 0xFE) ? 0 : 1;
      while (psrc < psrcmax && pdst < pdstmax) {
         *pdst++ = psrc[nreloff];
         psrc += 2;
      }

      // calc new netto size
      nres = pdst - pdstbeg;

      mtklog(("filtered %d ucs-2 chars", (int)nres));

      if (bfirst) gs.utf16read++;
   }

   return nres;
}

size_t Coi::readRaw(void *pbufin, size_t nBufSize)
{__
   // take cached data?
   if (data().src.data)
   {
      int nremain = data().src.size - data().src.index;
      int ntocopy = (int)nBufSize;
      if (ntocopy > nremain) ntocopy = nremain;
      if (ntocopy > 0) {
         memcpy(pbufin, data().src.data + data().src.index, ntocopy);
         data().src.index += ntocopy;
      }
      mtklog(("coi-read copied %d bytes from cache idx %d", ntocopy, (int)data().src.index));
      data().ntotalread += ntocopy;
      return ntocopy;
   }

   #ifdef VFILEBASE
   if (isHttp())  {
      size_t nread = rawReadHttpSubFile(pbufin, nBufSize);
      data().ntotalread += nread;
      return nread;
   }

   if (isFtp())   {
      size_t nread = rawReadFtpSubFile(pbufin, nBufSize);
      data().ntotalread += nread;
      return nread;
   }
   #endif // VFILEBASE


   // native file I/O:
   if (!data().pfile) {
      perr("read on non-open file: %s",name());
      return 0;
   }

   uchar *pBuf = (uchar *)pbufin;

   size_t nOffset = 0;
   size_t nRemain = nBufSize;
   while (nRemain > 0)
   {
      size_t nBlock = SFK_IO_BLOCK_SIZE;
      if (nBlock > nRemain) nBlock = nRemain;

      size_t nReadSub = fread(pBuf+nOffset, 1, nBlock, data().pfile); // unsafe.stdin
      if (nReadSub <= 0)
         break;

      nOffset += nReadSub;
      nRemain -= nReadSub;

      // if (nMax > 0)
      //    info.setProgress(nMax, nCur+nOffset, "bytes");
   }

   // if (nOffset > 0 && pmd5 != 0)
   //    pmd5->update(pBuf, nOffset);

   data().ntotalread += nOffset;

   return nOffset;
}

// rc0:ok >=0:failed to seek
int Coi::seek(num nOffset, int nOrigin)
{__
   if (nOrigin != SEEK_SET)
      return 9+perr("internal: seek: supports only SEEK_SET");

   #ifdef VFILEBASE
   if (!data().src.data && isVirtual()) // w/o root zips
   {
      // force caching of virtual files, except root zips
      if (preload("see", 0, 0)) // seek
         return 9+perr("seek failed: %s", name());
   }
   #endif // VFILEBASE

   // take cached data?
   if (data().src.data)
   {
      mtklog(("coi::seek to %d in cache", (int)nOffset));
      if (nOffset < 0 || nOffset > data().src.size)
         return 9+perr("cannot seek to position %s", numtoa(nOffset));
      data().src.index = nOffset;
      return 0;
   }

   #ifdef VFILEBASE
   // virtual file I/O: shouldn't be reached
   if (isHttp())  return 9+perr("cannot seek on http file");
   if (isFtp())   return 9+perr("cannot seek on ftp file");
   if (isZipSubEntry()) return 9+perr("cannot seek on zip entry");
   #endif // VFILEBASE

   // native file I/O:
   if (!data().pfile) {
      perr("seek on non-open file: %s",name());
      return 0;
   }

   FILE *f = data().pfile;

   #ifdef _WIN32
   if (sizeof(num) != sizeof(fpos_t)) return 9+perr("internal: myfseek: wrong fpos_t size, need 64 bits");
   return fsetpos(f, &nOffset);
   #else
   fpos64_t xpos;
   if (fgetpos64(f, &xpos)) // read xpos.__state
      return 9+perr("internal: seek: failed to read position");
    #if defined(MAC_OS_X) || defined(SOLARIS)
     xpos = (fpos_t)nOffset;
    #else
     if (sizeof(xpos.__pos) != sizeof(nOffset)) return 9+perr("internal: myfseek: wrong __pos size, need 64 bits");
     xpos.__pos = nOffset;
    #endif
   return fsetpos64(f, &xpos);
   #endif
}

size_t Coi::write(uchar *pBuf, size_t nBytes)
{__
   if (data().src.data)
      return 9+perr("write: conflict with cached data");

   #ifdef SFKPACK
   if (isOfficeSubEntry()) return 9+perr("cannot write on office entry");
   #endif // SFKPACK

   #ifdef VFILEBASE
   // virtual file I/O:
   if (isHttp())  return 9+perr("cannot write on http file");
   if (isFtp())   return 9+perr("cannot write on ftp file");
   if (isZipSubEntry()) return 9+perr("cannot write on zip entry");
   #endif // VFILEBASE

   // native file I/O:
   if (!data().pfile) {
      perr("write on non-open file: %s",name());
      return 0;
   }

   size_t nOffset = 0;
   size_t nRemain = nBytes;

   while (nRemain > 0)
   {
      size_t nBlock = SFK_IO_BLOCK_SIZE;
      if (nBlock > nRemain) nBlock = nRemain;

      size_t nWriteSub = fwrite(pBuf+nOffset, 1, nBlock, data().pfile);
      if (nWriteSub != nBlock)
         return nOffset+nWriteSub; // return no. of bytes actually written

      nOffset += nWriteSub;
      nRemain -= nWriteSub;

      // if (nMax > 0)
      //    info.setProgress(nMax, nCur+nOffset, "bytes");
   }

   // if (nOffset > 0 && pmd5 != 0)
   //    pmd5->update(pBuf, nOffset);

   return nOffset;
}

void Coi::close( )
{__
   mtklog(("coi-close %p %s", this, name()));

   // reset per-file statistics
   resetIOStatus();

   // assuming: close methods below do NOT use rbuf
   if (data().rbuf.data) {
      mtklog(("coi-close deletes rbuf %p size %d", data().rbuf.data, data().rbuf.getsize));
      delete [] data().rbuf.data;
   }
   memset(&data().rbuf, 0, sizeof(data().rbuf));
 
   if (data().src.data) {
      mtklog(("coi-close finished read from cache"));
      data().src.index = 0; // safety
      // return; // chg 1770
   }
 
   #ifdef VFILEBASE
   // http://thehost/thefile.txt
   if (isHttp())  { rawCloseHttpSubFile(); return; }
   if (isFtp())   { rawCloseFtpSubFile(); return; }
   #endif // VFILEBASE
 
 
   // in any case, reset the flag:
   data().bfileopen = 0;

   // native file I/O:
   if (!data().pfile) {
      mtklog(("close.file already done: %s",name()));
      return;
   }

   fclose(data().pfile);
   data().pfile = 0;
 
   // keeptime is applied only after data was written.
   // this may also happen later after a second open/close
   // on the same Coi to overwrite the input file.
   if (bClSetWriteCloseTime)
   {
      if (data().bwrite) {
         if (cs.debug)
            printf("applying -keeptime after write close: %s\n", name());
         int iSubRC = applyWriteCloseTime();
         if (iSubRC)
            pwarn("cannot set file time (rc=%d) for: %s\n",iSubRC,name());
      } else {
         if (cs.debug)
            printf("skipping -keeptime after non write close: %s\n", name());
      }
   }
}

int Coi::setKeepTime(Coi *pSrc)
{__
   // pSrc can also be == this

   // make sure filestat is read on src
   if (pSrc->getTime() <= 0)
      return 5; // does not apply

   // then copy whatever is set
   nClMTime = pSrc->nClMTime;
   nClCTime = pSrc->nClCTime;

   // mtklog(("coi.mtime: %u from setKeepTime",(uint)nClMTime));
 
   // and remember to apply this on write close
   bClSetWriteCloseTime = 1;

   return 0;
}

int Coi::applyWriteCloseTime( )
{__
   bClSetWriteCloseTime = 0;

   if (!data().bwrite)
      return 5; // ignored, file was not written

   #ifdef _WIN32

   HANDLE hDst = CreateFile(
      name(),
      FILE_WRITE_ATTRIBUTES,
      0,    // share
      0,    // security
      OPEN_EXISTING,
      bClDir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL,
      0     // template file
      );
   if (hDst == INVALID_HANDLE_VALUE)
      return 10;

   int nrc = 0;

   FILETIME nDstMTime, nDstCTime;
   FILETIME *pMTime=0, *pCTime=0;

   if (nClMTime > 0)
   {
      if (!makeWinFileTime(nClMTime, nDstMTime)) // coi.applyWriteCloseTime
         pMTime = &nDstMTime;
      else
         nrc = 11;
   }

   if (nClCTime > 0)
   {
      if (!makeWinFileTime(nClCTime, nDstCTime)) // coi.applyWriteCloseTime
         pCTime = &nDstCTime;
      else
         nrc = 12;
   }

   #ifdef WINFULL
   if (!nrc && (pMTime || pCTime)) {
      if (!SetFileTime(hDst, pCTime, 0, pMTime)) {
         perr("cannot set file time (rc=%u): %s\n", (uint)GetLastError(), name());
         nrc = 13;
      }
   }
   #endif
 
   CloseHandle(hDst);

   return nrc;

   #else

   // linux generic: set only mtime

   if (nClMTime <= 0)
      return 1; // nothing to set

   struct utimbuf otimes;
   mclear(otimes);

   otimes.modtime = nClMTime;

   int iRC = utime(name(), &otimes);
   if (iRC) return 9+perr("failed to set file times (rc=%d): %s\n",iRC,name());

   return 0;

   #endif
}

int Coi::openDir(int ilevel)
{__
   mtklog(("coi::opendir %p", this));

   #ifdef SFKPACK
   if (isOffice(105)) {
      int nrc = rawLoadDir(ilevel);
      if (nrc >= 5) return nrc; // failed
      data().bdiropen = 1;
      return 0;
   }
   #endif // SFKPACK

   #ifdef VFILEBASE
   if (isTravelZip(105) || isNet()) {
      int nrc = rawLoadDir(ilevel);
      if (nrc >= 5) return nrc; // failed
      data().bdiropen = 1;
      return 0;
   }
   #endif // VFILEBASE

   if (rawIsDir())
      return rawOpenDir();

   perr("no filesystem directory, cannot open: %s", name());
   return 9;
}

// caller MUST RELEASE COI after use!
Coi *Coi::nextEntry( )
{__
   mtklog(("coi::nextentry %p", this));

   // additional filter loop: block any kind of endless
   // link looping, i.e. dir links within a dir to itself.
   while (1)
   {
      Coi *psub = nextEntryRaw();
      if (!psub) return 0;
      if (psub == this) {
         mtklog(("coi.nextentry: skip sub == top for %p", psub));
         // psub is OWNED BY US, so release it first.
         if (!psub->decref())
            perr("int. #1310281137"); // should NOT happen
         // do NOT delete! it was a double ref onto THIS.
         continue;
      }
      if (!strcmp(psub->name(), name())) {
         mtklog(("coi.nextentry: skip equal sub %p, top %p for name %s", psub, this, psub->name()));
         // psub is OWNED BY US, so release it first.
         if (!psub->decref())
            delete psub; // no refs remaining, not cached
         continue;
      }
      // is different, return entry.
      return psub;
   }
   return 0; // not reached
}

// caller MUST RELEASE COI after use!
Coi *Coi::nextEntryRaw( )
{__

   #ifdef VFILEBASE
   if (isFtp())   return rawNextFtpEntry();
   #endif // VFILEBASE

   #ifdef SFKPACK
   if (isOffice(106)) return rawNextOfficeEntry();
   #endif // SFKPACK


   if (rawIsDir()) return rawNextEntry();

   perr("no directory, cannot read: %s", name());
   return 0;
}

void Coi::closeDir( )
{__
   mtklog(("coi::closedir %p", this));


   #ifdef VFILEBASE
   if (isFtp())   return rawCloseFtpDir();
   #endif // VFILEBASE

   #ifdef SFKPACK
   if (isOffice(107)) return rawCloseOfficeDir();
   #endif // SFKPACK


   if (rawIsDir())
      return rawCloseDir();

   perr("no directory, cannot close: %s", name());
}

int Coi::rawOpenDir( )
{
   // prepare traversal pattern, or simply
   // the directory name stripped from possible slash

   int nsize1 = strlen(name());
   if (data().pdirpat) delete [] data().pdirpat;
   data().pdirpat = new char[nsize1+12];
   #ifdef _WIN32
   joinPath(data().pdirpat, nsize1+8, name(), (char*)"*");
   #else
   joinPath(data().pdirpat, nsize1+8, name(), (char*)"");
   #endif
 
   #ifdef _WIN32

   // delay real actions until first nextEntry()

   #else

   // linux:
   if (!(data().ptrav = opendir(data().pdirpat)))
      return 1; // no such dir

   #endif

   data().bdiropen = 1;

   return 0;
}

bool isAbsolutePath(char *psz1)
{
   if (isHttpURL(psz1)) return 1; // 1770
   #ifdef _WIN32
   if (strlen(psz1) >= 2) {
      // try for C:\thedir
      char c1 = tolower(*psz1);
      if (c1 >= 'a' && c1 <= 'z' && *(psz1+1) == ':') // && *(psz1+2) == '\\')
         return 1;
   }
   #endif
   // e.g. /tmp
   // e.g. \\machine\path
   if (psz1[0] == glblPathChar) return 1;
   if (psz1[0] == '/') return 1; // sfk185 always
   return 0;
}

char *getAbsPathStart(char *pszin) // sfk1934
{
   static char szroot[SFK_MAX_PATH+10];

   strcopy(szroot,pszin);

   char *p = szroot;

   if (stribeg(p,"http:") || stribeg(p,"https://") || stribeg(p,"ftp:"))
   {
      // http://thehost.com:port/path
      char *psz=strstr(szroot,"://");
      if (!psz) return szroot;
      psz+=3;
      while (*psz!=0 && *psz!='/') psz++;
      if (!*psz) return szroot;
      // return: http://thehost.com:port/
      psz++; *psz='\0'; return szroot;
   }

   #ifdef _WIN32
   if (strlen(p) >= 2) {
      char c1 = tolower(*p);
      if (c1 >= 'a' && c1 <= 'z' && p[1] == ':') {
         // handle C:foo C:\foo
         p += 2; while (isUniPathChar(*p)) p++;
         // return: C: or "C:\"
         *p='\0'; return szroot;
      }
   }
   #endif

   // \foo.txt  ->  \   foo.txt
   // \\foo.txt ->  \\  foo.txt
   // foo.txt   ->      foo.txt
   while (isUniPathChar(*p)) p++;
   *p='\0';
   return szroot;
}

int joinPath(char *pszDst, int nMaxDst, char *pszSrc1, char *pszSrc2, int *pFlexible)
{
   mystrcopy(pszDst, pszSrc1, nMaxDst-4); // colon, terminator, 2 buffer.

   if (pFlexible)
   {
      // join c:\tvraw\ and ..\subdir\outfile.mts to c:\subdir\outfile.mts
      // join c:\tvraw\ and \tvedit\outfile.mts   to c:\tvedit\outfile.mts
      char szup[10];
      sprintf(szup, "..%c", glblPathChar);

      int iskipped=0;

      bool babs=(pszSrc2[0]==glblPathChar)?1:0;

      if (babs)
      {
         *pszDst = '\0';
         pszSrc2++;
         iskipped=1;

         #ifdef _WIN32
         if (pszSrc1[0]!=0 && pszSrc1[1]==':') {
            pszDst[0] = pszSrc1[0];
            pszDst[1] = pszSrc1[1];
            pszDst[2] = '\0';
         }
         #endif

         strcat(pszDst, glblPathStr);
      }

      int idstlen=strlen(pszDst);
 
      if (!babs)
      while (idstlen>0 && !strncmp(pszSrc2, szup, 3))
      {
         // c:\tvraw\ -> c:\tvraw
         // c:\ -> c:
         if (pszDst[idstlen]==glblPathChar)
            idstlen--;
         // c:\tvraw -> "c:\"
         while (idstlen>0 && pszDst[idstlen-1]!=glblPathChar
                #ifdef _WIN32
                && pszDst[idstlen-1]!=':'
                #endif
               )
               idstlen--;
         pszDst[idstlen]='\0';
         // ..\subdir\outfile.mts -> subdir\outfile.mts
         pszSrc2 += 3;
         iskipped += 3;
      }

      *pFlexible = iskipped;
   }

   int nlen = strlen(pszDst);
   if (nlen > 0) {
      char clast = pszDst[nlen-1];
      #ifdef _WIN32
      if (clast != ':') // AND below
      #endif
      if (clast != glblPathChar) {
         strcat(pszDst, glblPathStr);
         nlen++;
      }
   }
   if (nlen < nMaxDst)
      mystrcopy(pszDst+nlen, pszSrc2, nMaxDst-nlen);
   // printf("JOIN \"%s\" \"%s\" => \"%s\"\n",pszSrc1,pszSrc2,pszDst);
   return 0;
}

int joinShadowPath(char *pszDst, int nMaxDst, char *pszSrc1, char *pszSrc2)
{
   mystrcopy(pszDst, pszSrc1, nMaxDst-4); // colon, terminator, 2 buffer.
   int nlen = strlen(pszDst);
   if (nlen > 0) {
      char clast = pszDst[nlen-1];
      #ifdef _WIN32
      if (clast != ':') // AND below
      #endif
      if (clast != glblPathChar) {
         strcat(pszDst, glblPathStr);
         nlen++;
      }
   }
   mystrcopy(pszDst+nlen, "zz-shadow-01", (nMaxDst-2)-nlen);
   strcat(pszDst, glblPathStr);
   nlen = strlen(pszDst);
   mystrcopy(pszDst+nlen, pszSrc2, nMaxDst-nlen);
   // printf("JOIN \"%s\" \"%s\" => \"%s\"\n",pszSrc1,pszSrc2,pszDst);
   return 0;
}

#ifdef SFK_W64
intptr_t myfindfirst64(char *pszMask, SFKFindData *pout, int *pBadConv)
{
   if (!vname())
      return _findfirst64(pszMask, pout);

   sfkname oname1(pszMask);
   ushort *amask = oname1.wname();

   sfkfinddata64_t odata;
   mclear(odata);

   intptr_t pres = _wfindfirst64((const wchar_t *)amask, &odata);

   memset(pout, 0, sizeof(SFKFindData));

   sfkname oname2((ushort*)odata.name);
   strcopy(pout->name, oname2.vname()); // pout->name is an array
   if (pBadConv)
      *pBadConv = oname2.bbadconv;

   pout->attrib      = odata.attrib;
   pout->time_write  = odata.time_write;
   pout->time_create = odata.time_create;
   pout->size        = odata.size;

   return pres;
}

int myfindnext64(intptr_t phandle, SFKFindData *pout, int *pBadConv)
{
   if (!vname())
      return _findnext64(phandle, pout);

   sfkfinddata64_t odata;

   int ires = _wfindnext64(phandle, &odata);

   memset(pout, 0, sizeof(SFKFindData));

   sfkname oname((ushort*)odata.name);
   strcopy(pout->name, oname.vname()); // pout->name is an array
   if (pBadConv)
      *pBadConv = oname.bbadconv;

   // printf("VNAME.1: %s\n", dataAsTrace(odata.name,wcslen((wchar_t*)odata.name)*2));
   // printf("VNAME.2: %s\n", pout->name);

   pout->attrib      = odata.attrib;
   pout->time_write  = odata.time_write;
   pout->time_create = odata.time_create;
   pout->size        = odata.size;

   return ires;
}
#endif

// caller MUST RELEASE COI after use!
Coi *Coi::rawNextEntry( )
{
   if (!data().bdiropen) {
      perr("nextEntry() called without openDir()");
      return 0;
   }

   SFKFindData myfdat;

   char   szAbsName[SFK_MAX_PATH+10];

   int bBadConv = 0;

   // loop to skip ".", ".." and invalid files:
   while (1)
   {
      memset(&myfdat, 0, sizeof(myfdat));
      szAbsName[0] = 0;
      bBadConv = 0;
 
      #ifdef _WIN32

      // windows:
      if (!data().bdir1stdone) {
         data().bdir1stdone = 1;
         // first call:
         #ifdef SFK_W64
         data().otrav = myfindfirst64(data().pdirpat, &myfdat, &bBadConv);
         #else
          #ifndef _INTPTR_T_DEFINED
           typedef int intptr_t;
          #endif
         data().otrav = _findfirst(data().pdirpat, &myfdat);
         #endif
         if (data().otrav == -1)
            return 0; // probably empty dir
      } else {
         // subsequent calls
         #ifdef SFK_W64
         int nrc = myfindnext64(data().otrav, &myfdat, &bBadConv);
         #else
         int nrc = _findnext(data().otrav, &myfdat);
         #endif
         if (nrc) return 0; // no further entries
      }

      // printf("# rawnextentry attr %lxh %s\n", (uint)myfdat.attrib, myfdat.name);
 
      #else
 
      // linux:
      struct dirent *e = readdir(data().ptrav);
      if (e == NULL)
         return 0; // no further entries
 
      myfdat.name    = e->d_name;
      myfdat.attrib  = 0;

      #ifdef SOLARIS
      myfdat.rawtype = 0;
      #else
      myfdat.rawtype = (uint)e->d_type;
      #endif

      // dirent symbolic links have their own inode,
      // and are of no use to detect repeated content listings.
      //   myfdat.ninode  = (num)e->d_ino;
      //   myfdat.bhavenode = 1;
      // the actual inode is fetched below through stat64.
 
      #endif

      // always skip "." and ".."
      if (   !strcmp(myfdat.name, ".")
          || !strcmp(myfdat.name, ".."))
         continue;

      // construct absolute name of entry now,
      // although we may decide to skip the file.
      int nRootLen = strlen(name());
      int nSubLen  = strlen(myfdat.name);
 
      // create tmp absname, taking care of ":" and slash
      int nMixLen  = nRootLen+nSubLen;
      joinPath(szAbsName, SFK_MAX_PATH, name(), myfdat.name);

      // identify further file attributes:
      // symbolic link directory under linux?
 
      #ifndef _WIN32

      // get further dir/file statistics. no 32 bit compat here -
      // getting the latest g++ for linux shouldn't be too difficult.
      struct stat64 hStat1;
      if (stat64(szAbsName, &hStat1)) {
         if (cs.verbose) printf("nostat: %s (non-regular file)\n", szAbsName);
         cs.noFiles++;
         continue;
      }

      myfdat.rawmode = (uint)hStat1.st_mode;
      myfdat.rawnlnk = (uint)hStat1.st_nlink;

      // get the "true" (dereferenced) inode,
      // allowing dup content listing detection:
      myfdat.ninode  = (num)hStat1.st_ino;
      memcpy(&myfdat.ostdev, &hStat1.st_dev, sizeof(__dev_t));
      myfdat.bhavenode = 1;

      #ifdef SOLARIS
      myfdat.islink = 0; // set by hStat1 below
      myfdat.attrib = 0; // set by hStat1 below
      #else
      // DT_LNK: not available on older Linux versions
      if (e->d_type == DT_LNK)
         myfdat.islink = 1; // cannot tell here if dir or file link
      else
      if (e->d_type == DT_DIR)
         myfdat.attrib = 0x10; // dir
      else
      if (e->d_type == DT_REG)
         myfdat.attrib = 0x00; // regular file
      // else probably DT_UNKNOWN
      #endif
 
      // general linux, including older variants
      #ifdef S_IFLNK
      if ((hStat1.st_mode & S_IFLNK) == S_IFLNK)
         myfdat.islink = 1;
      #endif
      // no else here.
      if ((hStat1.st_mode & S_IFDIR) == S_IFDIR)
         myfdat.attrib = 0x10; // dir
      else
      if ((hStat1.st_mode & S_IFREG) == S_IFREG)
         myfdat.attrib = 0x00; // regular file
      else {
         if (cs.verbose) printf("nofile: %s (non-regular file)\n", myfdat.name);
         cs.noFiles++;
         continue;
      }
 
      /*
         NOTE: these are OCTAL VALUES, NOT hexadecimal.
         __S_IFDIR   0040000  // Directory.
         __S_IFCHR   0020000  // Character device.
         __S_IFBLK   0060000  // Block device.
         __S_IFREG   0100000  // Regular file.
         __S_IFIFO   0010000  // FIFO.
         __S_IFLNK   0120000  // Symbolic link.
         __S_IFSOCK  0140000  // Socket.
      */

      // skip of dirlinks should be done by caller
      // if (myfdat.islink && ((hStat1.st_mode & _S_IFDIR) == _S_IFDIR)) {
      //    // symbolic directory link:
      //    bIsDirLink = 1;
      //    if (cs.skipLinks && (nGlblFunc != eFunc_FileStat)) {
      //       continue;
      //    }
      // }
 
      // may also set 0x02 here for hidden files.
      // may also set 0x04 here for system files.
 
      myfdat.time_write  = hStat1.st_mtime;
      myfdat.time_create = hStat1.st_ctime;
      myfdat.size        = hStat1.st_size;
 
      #endif

      // entry valid, not skipped: take it
      break;

   }  // endwhile (1)

   // expect that we (the parent of psub) have a root
   // copied through from higher levels. if not,
   // set ourselves as the root.
   Coi *psub = new Coi(szAbsName, pszClRoot ? pszClRoot : name());
   psub->fillFrom(&myfdat);
   // time, size, hidden, link
   psub->bClBadName = bBadConv;

   #ifdef SFKDEEPZIP
   if (cs.probefiles)
      psub->probeFile();
   #endif // SFKDEEPZIP

   // verified file system directory:
   if (myfdat.attrib & 0x10)
      psub->bClFSDir = 1;

   psub->incref("rne");

   // read metadata successfully:
   psub->nClStatus = 1;

   return psub; // managed by caller
}

bool Coi::isHidden( ) {
   // TODO: so far, readStat can NOT determine the hidden status,
   //       therefore the call doesn't help unless this changes.
   if (!nClStatus) readStat('h');
   mtklog(("%d = coi::ishidden %s",bClHidden,name()));
   return bClHidden;
}

bool Coi::isLink   ( ) {
   // TODO: so far, readStat can NOT determine the link status,
   //       therefore the call doesn't help unless this changes.
   if (!nClStatus) readStat('l');
   return bClLink;
}

bool Coi::isDirLink( ) {
   // TODO: so far, readStat can NOT determine the link status,
   //       therefore the call doesn't help unless this changes.
   if (!nClStatus) readStat('l');
   return bClDir && bClLink;
}

void Coi::rawCloseDir( )
{
   if (!data().bdiropen)
      pwarn("closeDir() called on non-open dir");

   #ifdef _WIN32

   if (data().otrav != -1) {
      _findclose(data().otrav);
      data().otrav = -1;
   }

   #else

   if (data().ptrav) {
      closedir(data().ptrav);
      data().ptrav = 0;
   }

   #endif

   if (data().pdirpat) {
      delete [] data().pdirpat;
      data().pdirpat = 0;
   }

   data().bdiropen = 0;
}

CoiTable::CoiTable() {
   mtklog(("coitab ctr %p",this));
   nClArraySize = 0;
   nClArrayUsed = 0;
   apClArray    = 0;
}

CoiTable::~CoiTable() {
   mtklog(("coitab dtr %p",this));
   resetEntries();
}

void CoiTable::resetEntries()
{
   if (apClArray)
   {
      for (int i=0; i<nClArrayUsed; i++)
      {
         if (apClArray[i])
         {
            if (apClArray[i]->refcnt() > 0) {
               // should not occur as CoiTable adds copies of coi.
               perr("cannot cleanup used file object: %d %s\n", apClArray[i]->refcnt(), apClArray[i]->name());
            } else {
               delete apClArray[i];
            }
         }
         apClArray[i] = 0;
      }
   }
   nClArrayUsed = 0;
   if (apClArray)
      delete [] apClArray;
   apClArray = 0;
   nClArraySize = 0;
}

int CoiTable::numberOfEntries() { return nClArrayUsed; }

bool CoiTable::isSet(int iIndex) {
   if (iIndex < 0) { pwarn("illegal index: %d\n", iIndex); return 0; }
   return (iIndex < nClArrayUsed) ? 1 : 0;
}

int CoiTable::expand(int nSoMuch) {
   Coi **apTmp = new Coi*[nClArraySize+nSoMuch];
   if (!apTmp) return 9;
   if (apClArray) {
      memcpy(apTmp, apClArray, nClArraySize*sizeof(Coi*));
      delete [] apClArray;
   }
   apClArray = apTmp;
   nClArraySize += nSoMuch;
   return 0;
}

int CoiTable::hasEntry(char *pszFilename) {
   for (int i=0; i<nClArrayUsed; i++) {
      Coi *p = apClArray[i];
      if (p!=0 && strcmp(pszFilename, p->name())==0)
         return 1;
   }
   return 0;
}

// add a COPY of the supplied coi
int CoiTable::addEntry(Coi &ocoi, int nAtPos) {
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }
   if (nAtPos != -1) {
      for (int i=nClArrayUsed; i>nAtPos; i--)
         apClArray[i] = apClArray[i-1];
      apClArray[nAtPos] = ocoi.copy();
      nClArrayUsed++;
   } else {
      apClArray[nClArrayUsed++] = ocoi.copy();
   }
   return 0;
}

int CoiTable::addSorted(Coi &ocoi, char cSortedBy, bool bUseCase)
{
   if (nClArrayUsed == nClArraySize) {
      if (nClArraySize == 0) {
         if (expand(10)) return 9;
      } else {
         if (expand(nClArraySize)) return 9;
      }
   }

   Coi *padd = &ocoi;

   int nInsPos = -1;
   int nCnt = numberOfEntries();

   #if 1 // sfk1963: binary sort with sfk big, sfk late etc.

   int nbot  = 0;
   int ntop  = nCnt;
   int ndist = ntop - nbot;
   int nmid  = 0;
   Coi *pown = 0;

   // printf("addsorted %u %c cnt=%d\n",(int)padd->getSize(),cSortedBy,nCnt);

   while (ndist > 0)
   {
      int nhalf = ndist / 2;
      nmid = nbot + nhalf;
      // printf("from %02u to %02u mid %02u for %u\n",nbot,ntop,nmid,(int)padd->getSize());
      num ncmp  = 0;
      pown = apClArray[nmid];
      switch (cSortedBy)
      {
         case 'T': ncmp = padd->getTime() - pown->getTime(); break;
         case 't': ncmp = pown->getTime() - padd->getTime(); break;
         case 'S': ncmp = padd->getSize() - pown->getSize(); break;
         case 's': ncmp = pown->getSize() - padd->getSize(); break;
         case 'N': {
            ncmp = bUseCase ?   strcmp(padd->name(), pown->name())
                              : mystricmp(padd->name(), pown->name());
            break;
         }
         case 'n': {
            ncmp = bUseCase ?   strcmp(padd->name(), pown->name())
                              : mystricmp(padd->name(), pown->name());
            break;
         }
         default: return 9+perr("internal #141272257\n");
      }
      if (ncmp > 0) {
         nmid++;
         if (nbot == nmid) break;
         nbot = nmid; // printf("   set bot %u\n", nbot);
      }
      if (ncmp < 0) {
         if (ntop == nmid) break;
         ntop = nmid; // printf("   set top %u\n", ntop);
      }
      if (ncmp == 0) {
         nmid++;
         // printf("match\n");
         break;
      }
      ndist = ntop - nbot;
   }
   // printf("   have nmid %u\n", nmid);
   if (nmid > 0 && nmid <= nCnt)
      nInsPos = nmid;
   else
      nInsPos = 0;

   #else

   int i=0; bool bbail=0;
   for (i=0; i<nCnt; i++)
   {
      Coi *pown = apClArray[i];
      switch (cSortedBy) {
         case 'T': bbail = (pown->getTime() > padd->getTime()); break;
         case 't': bbail = (pown->getTime() < padd->getTime()); break;
         case 'S': bbail = (pown->getSize() > padd->getSize()); break;
         case 's': bbail = (pown->getSize() < padd->getSize()); break;
         case 'N': {
            int ncmp = bUseCase ?      strcmp(pown->name(), padd->name())
                                   : mystricmp(pown->name(), padd->name());
            bbail = (ncmp > 0);
            break;
         }
         case 'n': {
            int ncmp = bUseCase ?      strcmp(pown->name(), padd->name())
                                   : mystricmp(pown->name(), padd->name());
            bbail = (ncmp < 0);
            break;
         }
         default: return 9+perr("internal #141272256\n");
      }
      if (bbail) break;
   }
   if (i < nCnt) nInsPos = i;

   #endif

   // printf("   addEntry at %u\n",(int)nInsPos);

   int irc = addEntry(ocoi, nInsPos);

   // nCnt = numberOfEntries();
   // for (int i=0; i<nCnt; i++) printf("   %d\n",(int)apClArray[i]->getSize());

   return irc;
}

int CoiTable::removeEntry(int nAtPos) {
   if (nAtPos < 0 || nAtPos >= nClArrayUsed)
      return 9;
   if (apClArray[nAtPos]) delete apClArray[nAtPos];
   for (int i=nAtPos; i<nClArrayUsed-1; i++)
      apClArray[i] = apClArray[i+1];
   apClArray[nClArrayUsed-1] = 0; // just in case
   nClArrayUsed--;
   return 0;
}

int CoiTable::setEntry(int nIndex, Coi *pcoi) {
   if (nIndex >= nClArrayUsed)
      return 9+perr("illegal set index: %d\n", nIndex);
   if (apClArray[nIndex])
      delete apClArray[nIndex];
   apClArray[nIndex] = pcoi ? pcoi->copy() : 0;
   return 0;
}

Coi *CoiTable::getEntry(int nIndex, int nTraceLine) {
   if (nIndex >= 0 && nIndex < nClArrayUsed)
      return apClArray[nIndex];
   perr("illegal CoiTable index: %d tline %d\n", nIndex, nTraceLine);
   return 0;
}

// - - - console output

ProgressInfo info;

void initConsole()
{
   // manually set or override console width:
   char *psz2 = getenv("SFK_CONFIG"); // console
   if (psz2) {
      psz2 = strstr(psz2, "columns:");
      if (psz2) {
         int ncols = atol(psz2+8);
         if (ncols >= 40) {
            nGlblConsColumns = ncols;
            bGlblConsColumnsSet = 2; // change forbidden
            info.setWidth(nGlblConsColumns);
         }
      }
   }

   // need this also for html help
   char *pszColEnv = getenv("SFK_COLORS");
   if (pszColEnv)
      setColorScheme(pszColEnv);

   #ifdef WINFULL

   hGlblConsole = GetStdHandle(STD_OUTPUT_HANDLE);
   hGlblStdIn   = GetStdHandle(STD_INPUT_HANDLE);

   // TODO: vc10 with GetConsoleScreenBufferInfoEx
   CONSOLE_SCREEN_BUFFER_INFO oConInf;
   if (!GetConsoleScreenBufferInfo(hGlblConsole, &oConInf)) {
      // not in interactive mode, e.g. output redirected to file:
      gs.usecolor = gs.usehelpcolor = 0;
      cs.usecolor = cs.usehelpcolor = 0;
      return;
   }
   bGlblHaveInteractiveConsole = 1;
   if (oConInf.dwCursorPosition.X==0 && oConInf.dwCursorPosition.Y==0)
      bGlblStartedInEmptyConsole = 1;
   nGlblConsAttrib = oConInf.wAttributes;
 
   /*
   printf("CONSATTRIB DEFAULTS %lxh:\n",nGlblConsAttrib);
   printf("   %u  BACKGROUND_BLUE\n" , nGlblConsAttrib & BACKGROUND_BLUE);
   printf("   %u  BACKGROUND_GREEN\n", nGlblConsAttrib & BACKGROUND_GREEN);
   printf("   %u  BACKGROUND_RED\n"  , nGlblConsAttrib & BACKGROUND_RED);
   */

   int nConCol = oConInf.srWindow.Right - oConInf.srWindow.Left + 1;
   int nConRow = oConInf.srWindow.Bottom - oConInf.srWindow.Top + 1;

   if (!bGlblConsColumnsSet && (oConInf.dwSize.X >= 60)) {
      nGlblConsColumns = oConInf.dwSize.X;
      bGlblConsColumnsSet = 1;
      info.setWidth(nGlblConsColumns);
   }
   if (!bGlblConsRowsSet && (nConRow >= 10)) {
      nGlblConsRows = nConRow;
      bGlblConsRowsSet = 1;
   }

   if (!pszColEnv)
   {
      // if we autodetect a black background shell
      uint nBackMask = BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE;
      if ((nGlblConsAttrib & (nBackMask)) == 0) {
         // auto-select color theme:black
         setColorScheme("theme:black");
      }
      if ((nGlblConsAttrib & (nBackMask)) == nBackMask) {
         // optimize colors for white background
         setColorScheme("theme:white");
      }
   }
   #endif

   #ifdef _WIN32

   #ifdef WINFULL
   if (gs.usecolor || cs.usecolor)
      SetConsoleCtrlHandler(ctrlcHandler, 1);
   #endif

   #else

   signal(SIGINT, ctrlcHandler);

   #endif
}

// sfk1972 console auto width: this called 2/sec with
// commands showing a long operation status, like copy
void updateConsole()
{
   #ifdef WINFULL

   if (hGlblConsole == 0
       || bGlblHaveInteractiveConsole == 0)
      return;

   if (bGlblConsColumnsSet == 0     // access didnt work
       || bGlblConsColumnsSet == 2) // change forbidden
         return;

   CONSOLE_SCREEN_BUFFER_INFO oConInf;
   if (!GetConsoleScreenBufferInfo(hGlblConsole, &oConInf))
      return;

   if (oConInf.dwSize.X >= 60) {
      nGlblConsColumns = oConInf.dwSize.X;
      info.setWidth(nGlblConsColumns);
   }

   #endif
}

int autoCalcWrapColumns()
{
   int ncols = 80;
   if (bGlblConsColumnsSet)
   {
      if (cs.verbose >= 2) printf("ConsoleColumns=%d\n",nGlblConsColumns);
      return nGlblConsColumns-2;
   }
   #ifdef _WIN32
   HWND hDeskWin = GetDesktopWindow();
   if (hDeskWin) {
      HDC hdcDesk = GetWindowDC(hDeskWin);
      if (hdcDesk != NULL) {
         int ndeskw = GetDeviceCaps(hdcDesk, HORZRES); // gdi32.lib
         int ncols2 = ndeskw / 10; // default char width: 10 pixels
         if (ncols2 > 80)
            ncols = ncols2;
         if (cs.verbose >= 2) printf("wdesk %d col.calc %d used %d\n",ndeskw,ncols2,ncols);
         ReleaseDC(hDeskWin, hdcDesk);
      }
   }
   #else
   // identify number of linux shell columns
   #endif
   return ncols-2;
}

ProgressInfo::ProgressInfo()
{
   memset(this, 0, sizeof(ProgressInfo));
   setWidth(80);
   nLastDumpTime = getCurrentTime();
   bAddInfoPrio  = 1;
}

void ProgressInfo::setWidth(int nColumns) {
   nMaxChars      = nColumns  -  3;
   nMaxSubChars   = nMaxChars - 20;
   // sfk1972 console auto width
   if (nMaxChars > (nAddInfoCols+10) + 30)
      nMaxSubChars = nMaxChars - (nAddInfoCols+10);
}

void ProgressInfo::setAddInfoWidth(int nAddCols) {
   nAddInfoCols = nAddCols;
   // vrfy  58% [filename]......... addinfo
   // 1234567890                    addcols
   nAddCols += 10; // add left side columns
   if (nMaxChars > nAddCols + 30)
      nMaxSubChars = nMaxChars - nAddCols;
}

void ProgressInfo::fixAddInfoWidth() {
   if ((int)strlen(szAddInfo) > nAddInfoCols) {
      nAddInfoCols = (int)strlen(szAddInfo);
      int nAddCols = nAddInfoCols + 10;
      if (nMaxChars > nAddCols + 30)
         nMaxSubChars = nMaxChars - nAddCols;
   }
}

void ProgressInfo::setStatus(cchar *pverb, cchar *psubj, cchar *pszAddInfo, int nKeepFlags)
{
   strcopy(szVerb, pverb);
   strcopy(szSubject, psubj);
   if (nKeepFlags & eKeepProg)  // keep progress
      szPerc[0] = '\0';
   if (pszAddInfo) {
      if (!strcmp(pszAddInfo, "00")) {
         strcpy(szPerc, "... ");
         strcopy(szAddInfo, pszAddInfo+2);
      } else
         strcopy(szAddInfo, pszAddInfo);
   }
   else
   if (!(nKeepFlags & eKeepAdd)) { // keep addinfo
      szAddInfo[0] = '\0';
   }

   fixAddInfoWidth();

   if (nKeepFlags & eSlowCycle)
      nLastDumpTime = getCurrentTime();

   if (!(nKeepFlags & eNoCycle))
      cycle();
}

void ProgressInfo::setStatProg(cchar *pverb, cchar *psubj, num nMax, num nCur, cchar *pszUnit) {
   strcopy(szVerb, pverb);
   strcopy(szSubject, psubj);
   szAddInfo[0] = '\0';
   setProgress(nMax, nCur, pszUnit);
}

void ProgressInfo::setProgress(num nMax, num nCur, cchar *pszUnit, bool btriple)
{
   if (nMax <= 0) nMax = 1; // safe division
   int nPerc = (int)(nCur * 100 / nMax);
   if (nPerc > 0 && nPerc <= 100) {
      if (btriple)
         sprintf(szPerc, "%03d%% ", nPerc);
      else
         sprintf(szPerc, "%02d%% ", nPerc);
   }
   else
   if (nPerc > 100) {
      sprintf(szPerc, "100%% ");
      if (cs.debug) printf("[progress: cur=%d max=%d perc=%d]\n", (int)nCur, (int)nMax, (int)nPerc);
   } else {
      if (btriple)
         sprintf(szPerc, ".... ");
      else
         sprintf(szPerc, "... ");
      if (cs.debug) printf("[progress: cur=%d max=%d perc=%d]\n", (int)nCur, (int)nMax, (int)nPerc);
   }
   cycle();
}

void ProgressInfo::clearProgress( )
{
   szPerc[0] = '\0';
}

void ProgressInfo::cycle() {
   #ifdef _WIN32
   // windows: if output is redirected, info display makes no sense.
   if (!bGlblHaveInteractiveConsole)
      return;
   #endif
   if (cs.quiet || cs.noprog)
      return;
   if (getCurrentTime() >= (nLastDumpTime + 500)) {
      updateConsole();  // sfk1972 internal
      dumpTermStatus();
  }
}

void ProgressInfo::setAction(cchar *pverb, cchar *psubj, cchar *pszAddInfo, int nKeepFlags) {
   setStatus(pverb, psubj, pszAddInfo, nKeepFlags);
   if (!(nKeepFlags & eNoPrint))
      print();
}

void ProgressInfo::print() {
   if (cs.quiet || cs.noprog)
      return;
   dumpTermStatus();
}

void ProgressInfo::printLine(int nFilter) {
   if (cs.quiet || cs.noprog)
      return;
   if (nFilter & (1<<1)) {
      // print only subject, but with unlimited length
      clearTermStatus();
      oprintf("%s\n", szSubject);
   } else {
      if (nFilter & (1<<2))
         strcpy(szPerc, " ");
      dumpTermStatus(); // may clear previous status output
      printf("\n");
   }
   nDumped = 0;
}

int ProgressInfo::print(const char *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBuf, sizeof(szPrintBuf)-10, pszFormat, argList);
   szPrintBuf[sizeof(szPrintBuf)-10] = '\0';
   clear();
   printf("%s", szPrintBuf);
   return 0;
}

void ProgressInfo::setAddInfo(const char *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szAddInfo, sizeof(szAddInfo)-10, pszFormat, argList);
   szAddInfo[sizeof(szAddInfo)-10] = '\0';
   fixAddInfoWidth();
}

void ProgressInfo::clear()
{
   clearTermStatus();
   szVerb[0]      = '\0';
   szSubject[0]   = '\0';
   szAddInfo[0]   = '\0';
   szPerc[0]      = '\0';
   // nLastDumpTime  = getCurrentTime();
}

void ProgressInfo::clearTermStatus()
{
   if (nDumped > 0 && nDumped < sizeof(szTermBuf)-4)
   {
      mtklog(("info::clearterm"));
      memset(szTermBuf, ' ', nDumped);
      szTermBuf[nDumped] = '\0';
      printf("%s\r",szTermBuf);
      fflush(stdout);
      nDumped = 0;
   }
   // do NOT reset szVerb etc. here
}

void ProgressInfo::dumpTermStatus()
{__
   if (cs.quiet || cs.noprog)
      return;

   if (!szSubject[0])
   {
      if (nDumped)
         clearTermStatus();
      return;  // nothing to dump
   }

   int nToClear = nDumped;

   int nMaxMiddle = nMaxSubChars;
   if (!szPerc[0])
      nMaxMiddle += 2; // no percentage: have more chars to use

   // if there is few space, and low addinfo prio
   bool bWithAdd = 1;
   if ((int)strlen(szSubject) > nMaxMiddle) {
      if (!bAddInfoPrio) {
         bWithAdd = 0;
         nMaxMiddle += nAddInfoReserve; // add space to center
      }
   }

   if ((int)strlen(szSubject) > nMaxMiddle) {
      int nlen  = strlen(szSubject);
      int nmax2 = nMaxMiddle - 3;
      if (nmax2 < 0) nmax2 = 0;
      char *psz1 = &szSubject[nlen-nmax2];
      snprintf(szTermBuf, sizeof(szTermBuf)-10, "...%s", psz1);
   } else {
      snprintf(szTermBuf, sizeof(szTermBuf)-10, "%-*.*s", (int)nMaxMiddle, (int)nMaxMiddle, szSubject);
   }

   nDumped = 0;

   char cTurn = (nTurn & 1) ? '.':' ';
   nTurn++;

   if (szVerb[0]) {
      int nvlen = strlen(szVerb);
      if (nvlen > 5) nvlen = 5;
      while (nvlen > 3 && szVerb[nvlen-1] == ' ')
         nvlen--;
      if (szPerc[0]) {
         cchar *pszPerc2 = szPerc;
         if (!strcmp(szPerc, " "))
            pszPerc2 = ""; // ignore, is just a dummy
         printx("<head>%-*.*s %s<def>", nvlen,nvlen, szVerb, pszPerc2);
         nDumped += 6 + strlen(szPerc);
      } else {
         printx("<head>%-*.*s <time>%c<def> ", nvlen,nvlen, szVerb, cTurn);
         nDumped += 6 + 2;
      }
   }

   mtklog(("info::dump \"%s\"", szTermBuf));
   printf("%s ",szTermBuf);
   nDumped += strlen(szTermBuf) + 1;

   if (bWithAdd && strlen(szAddInfo)) {
      setTextColor(nGlblTimeColor);
      printf("%s", szAddInfo);
      setTextColor(-1);
      nDumped += strlen(szAddInfo);
   }
 
   // need to add a blank area to clear old output?
   int nDelta = nToClear - nDumped;
   if (nDelta > 0 && nDelta < sizeof(szTermBuf)-4)
   {
      memset(szTermBuf, ' ', nDelta);
      szTermBuf[nDelta] = '\0';
      printf("%s",szTermBuf);
   }

   printf("\r");
   fflush(stdout);
   nLastDumpTime = getCurrentTime();
}

class InfoCounter {
public:
   InfoCounter    ( );
   uint count    ( );  // RC > 0 says print the counter now
   bool  checkTime( );  // RC > 0 says print the counter now
   bool  countSkip(char *pszFile);
   uint value    ( );
   uint skipped  ( );
   char  *skipInfo( );
   void  reset    ( );
private:
   void  copyAll  ( );

   uint nClUnits;
   uint nClSkipped;
   num   nClTime;

   uint nClTellSteps;
   uint nClLastTold;
   uint nClTellSteps2;
   uint nClLastTold2;
   num   nClLastTimeTold;
   int  nClTimeInertia; // to avoid calling getCurrentTime() too often

   char  aClSkipInfo[100];
   int  nClSkipIdx;
};

InfoCounter::InfoCounter() { reset(); }

void InfoCounter::reset()
{
   nClUnits      = 0;
   nClSkipped    = 0;
   nClTime       = 0;

   nClLastTold   = 0;
   nClTellSteps  = 1;
   nClLastTold2  = 0;
   nClTellSteps2 = 1;
   nClLastTimeTold = 0;
   nClTimeInertia = 10;

   memset(aClSkipInfo, 0, sizeof(aClSkipInfo));
   // memset(aClSkipInfo, ' ', 5 * 3);
   nClSkipIdx = 2; // force switch to 0 on first add
}

void InfoCounter::copyAll() {
   nClLastTimeTold = nClTime;
   nClLastTold     = nClUnits;
   nClLastTold2    = nClSkipped;
   nClTimeInertia  = 10;
}

uint InfoCounter::value()    { return nClUnits; }
uint InfoCounter::skipped()  { return nClSkipped; }
char *InfoCounter::skipInfo() { return aClSkipInfo; }

uint InfoCounter::count() {
   nClUnits++;
   if (nClUnits >= (nClLastTold+nClTellSteps)) {
      nClTellSteps++;
      copyAll();
      return 1;
   }
   return checkTime();
}

bool InfoCounter::countSkip(char *pszFile)
{
   nClSkipped++;

   // mtklog(("countskip %s", pszFile));

   // update list of skipped file extensions
   char *pszInfo = pszFile;
   char *pszExt  = strrchr(pszInfo, '.');
   if (pszExt) pszInfo = pszExt;
   int nInfoLen = strlen(pszInfo);
   if (nInfoLen > 4) pszInfo = pszInfo + nInfoLen - 4;
   char *pszCur = &aClSkipInfo[5*nClSkipIdx];
   nInfoLen = strlen(pszInfo);
   if (   strncmp(&aClSkipInfo[ 0], pszInfo, nInfoLen)
       && strncmp(&aClSkipInfo[ 5], pszInfo, nInfoLen)
       && strncmp(&aClSkipInfo[10], pszInfo, nInfoLen)
      )
   {
      // there is a change, so step and write
      nClSkipIdx = (nClSkipIdx + 1) % 3;
      pszCur = &aClSkipInfo[5*nClSkipIdx];
      memset(pszCur, ' ', 5);
      strncpy(pszCur, pszInfo, nInfoLen);
      if (nClSkipIdx == 2)
         pszCur[4] = '\0';
   }

   // mtklog(("countskip info %s", aClSkipInfo));

   if (nClSkipped >= (nClLastTold2+nClTellSteps2)) {
      nClTellSteps2++;
      copyAll();
      return 1;
   }
   return checkTime();
}

bool InfoCounter::checkTime() {
   if (nClTimeInertia-- > 0)
      return 0;
   nClTimeInertia = 10;
   nClTime = getCurrentTime();
   if (nClTime > nClLastTimeTold + 1000) {
      copyAll();
      return 1;
   }
   return 0;
}

InfoCounter glblFileCount;

void resetFileCounter()   { glblFileCount.reset(); cs.lines = 0; }
bool fileCountCheckTime() { return glblFileCount.checkTime(); }
uint glblFileCountValue() { return glblFileCount.value(); }

// - - - code page support and case insensitive string comparison

/*
   middle europe: 852 1250
   cyrillic     : 866 1251
   west.  europe: 850 1252
*/

// :middle europe
unsigned short glblCodePage1250[] = {
   0x80,0x20ac, 0x82,0x201a, 0x84,0x201e, 0x85,0x2026, 0x86,0x2020, 0x87,0x2021, 0x89,0x2030, 0x8a,0x0160,
   0x8b,0x2039, 0x8c,0x015a, 0x8d,0x0164, 0x8e,0x017d, 0x8f,0x0179, 0x91,0x2018, 0x92,0x2019, 0x93,0x201c,
   0x94,0x201d, 0x95,0x2022, 0x96,0x2013, 0x97,0x2014, 0x99,0x2122, 0x9a,0x0161, 0x9b,0x203a, 0x9c,0x015b,
   0x9d,0x0165, 0x9e,0x017e, 0x9f,0x017a, 0xa1,0x02c7, 0xa2,0x02d8, 0xa3,0x0141, 0xa5,0x0104, 0xaa,0x015e,
   0xaf,0x017b, 0xb2,0x02db, 0xb3,0x0142, 0xb9,0x0105, 0xba,0x015f, 0xbc,0x013d, 0xbd,0x02dd, 0xbe,0x013e,
   0xbf,0x017c, 0xc0,0x0154, 0xc3,0x0102, 0xc5,0x0139, 0xc6,0x0106, 0xc8,0x010c, 0xca,0x0118, 0xcc,0x011a,
   0xcf,0x010e, 0xd0,0x0110, 0xd1,0x0143, 0xd2,0x0147, 0xd5,0x0150, 0xd8,0x0158, 0xd9,0x016e, 0xdb,0x0170,
   0xde,0x0162, 0xe0,0x0155, 0xe3,0x0103, 0xe5,0x013a, 0xe6,0x0107, 0xe8,0x010d, 0xea,0x0119, 0xec,0x011b,
   0xef,0x010f, 0xf0,0x0111, 0xf1,0x0144, 0xf2,0x0148, 0xf5,0x0151, 0xf8,0x0159, 0xf9,0x016f, 0xfb,0x0171,
   0xfe,0x0163, 0xff,0x02d9,
   0,0
};

unsigned short glblCodePage852[] = {
   0x80,0x00c7, 0x81,0x00fc, 0x82,0x00e9, 0x83,0x00e2, 0x84,0x00e4, 0x85,0x016f, 0x86,0x0107, 0x87,0x00e7,
   0x88,0x0142, 0x89,0x00eb, 0x8a,0x0150, 0x8b,0x0151, 0x8c,0x00ee, 0x8d,0x0179, 0x8e,0x00c4, 0x8f,0x0106,
   0x90,0x00c9, 0x91,0x0139, 0x92,0x013a, 0x93,0x00f4, 0x94,0x00f6, 0x95,0x013d, 0x96,0x013e, 0x97,0x015a,
   0x98,0x015b, 0x99,0x00d6, 0x9a,0x00dc, 0x9b,0x0164, 0x9c,0x0165, 0x9d,0x0141, 0x9e,0x00d7, 0x9f,0x010d,
   0xa0,0x00e1, 0xa1,0x00ed, 0xa2,0x00f3, 0xa3,0x00fa, 0xa4,0x0104, 0xa5,0x0105, 0xa6,0x017d, 0xa7,0x017e,
   0xa8,0x0118, 0xa9,0x0119, 0xaa,0x00ac, 0xab,0x017a, 0xac,0x010c, 0xad,0x015f, 0xae,0x00ab, 0xaf,0x00bb,
   0xb0,0x2591, 0xb1,0x2592, 0xb2,0x2593, 0xb3,0x2502, 0xb4,0x2524, 0xb5,0x00c1, 0xb6,0x00c2, 0xb7,0x011a,
   0xb8,0x015e, 0xb9,0x2563, 0xba,0x2551, 0xbb,0x2557, 0xbc,0x255d, 0xbd,0x017b, 0xbe,0x017c, 0xbf,0x2510,
   0xc0,0x2514, 0xc1,0x2534, 0xc2,0x252c, 0xc3,0x251c, 0xc4,0x2500, 0xc5,0x253c, 0xc6,0x0102, 0xc7,0x0103,
   0xc8,0x255a, 0xc9,0x2554, 0xca,0x2569, 0xcb,0x2566, 0xcc,0x2560, 0xcd,0x2550, 0xce,0x256c, 0xcf,0x00a4,
   0xd0,0x0111, 0xd1,0x0110, 0xd2,0x010e, 0xd3,0x00cb, 0xd4,0x010f, 0xd5,0x0147, 0xd6,0x00cd, 0xd7,0x00ce,
   0xd8,0x011b, 0xd9,0x2518, 0xda,0x250c, 0xdb,0x2588, 0xdc,0x2584, 0xdd,0x0162, 0xde,0x016e, 0xdf,0x2580,
   0xe0,0x00d3, 0xe1,0x00df, 0xe2,0x00d4, 0xe3,0x0143, 0xe4,0x0144, 0xe5,0x0148, 0xe6,0x0160, 0xe7,0x0161,
   0xe8,0x0154, 0xe9,0x00da, 0xea,0x0155, 0xeb,0x0170, 0xec,0x00fd, 0xed,0x00dd, 0xee,0x0163, 0xef,0x00b4,
   0xf0,0x00ad, 0xf1,0x02dd, 0xf2,0x02db, 0xf3,0x02c7, 0xf4,0x02d8, 0xf5,0x00a7, 0xf6,0x00f7, 0xf7,0x00b8,
   0xf8,0x00b0, 0xf9,0x00a8, 0xfa,0x02d9, 0xfb,0x0171, 0xfc,0x0158, 0xfd,0x0159, 0xfe,0x25a0, 0xff,0x00a0,
   0,0
};

// :cyrillic
unsigned short glblCodePage1251[] = {
   0x80,0x0402, 0x81,0x0403, 0x82,0x201a, 0x83,0x0453, 0x84,0x201e, 0x85,0x2026, 0x86,0x2020, 0x87,0x2021,
   0x88,0x20ac, 0x89,0x2030, 0x8a,0x0409, 0x8b,0x2039, 0x8c,0x040a, 0x8d,0x040c, 0x8e,0x040b, 0x8f,0x040f,
   0x90,0x0452, 0x91,0x2018, 0x92,0x2019, 0x93,0x201c, 0x94,0x201d, 0x95,0x2022, 0x96,0x2013, 0x97,0x2014,
   0x99,0x2122, 0x9a,0x0459, 0x9b,0x203a, 0x9c,0x045a, 0x9d,0x045c, 0x9e,0x045b, 0x9f,0x045f, 0xa1,0x040e,
   0xa2,0x045e, 0xa3,0x0408, 0xa5,0x0490, 0xa8,0x0401, 0xaa,0x0404, 0xaf,0x0407, 0xb2,0x0406, 0xb3,0x0456,
   0xb4,0x0491, 0xb8,0x0451, 0xb9,0x2116, 0xba,0x0454, 0xbc,0x0458, 0xbd,0x0405, 0xbe,0x0455, 0xbf,0x0457,
   0xc0,0x0410, 0xc1,0x0411, 0xc2,0x0412, 0xc3,0x0413, 0xc4,0x0414, 0xc5,0x0415, 0xc6,0x0416, 0xc7,0x0417,
   0xc8,0x0418, 0xc9,0x0419, 0xca,0x041a, 0xcb,0x041b, 0xcc,0x041c, 0xcd,0x041d, 0xce,0x041e, 0xcf,0x041f,
   0xd0,0x0420, 0xd1,0x0421, 0xd2,0x0422, 0xd3,0x0423, 0xd4,0x0424, 0xd5,0x0425, 0xd6,0x0426, 0xd7,0x0427,
   0xd8,0x0428, 0xd9,0x0429, 0xda,0x042a, 0xdb,0x042b, 0xdc,0x042c, 0xdd,0x042d, 0xde,0x042e, 0xdf,0x042f,
   0xe0,0x0430, 0xe1,0x0431, 0xe2,0x0432, 0xe3,0x0433, 0xe4,0x0434, 0xe5,0x0435, 0xe6,0x0436, 0xe7,0x0437,
   0xe8,0x0438, 0xe9,0x0439, 0xea,0x043a, 0xeb,0x043b, 0xec,0x043c, 0xed,0x043d, 0xee,0x043e, 0xef,0x043f,
   0xf0,0x0440, 0xf1,0x0441, 0xf2,0x0442, 0xf3,0x0443, 0xf4,0x0444, 0xf5,0x0445, 0xf6,0x0446, 0xf7,0x0447,
   0xf8,0x0448, 0xf9,0x0449, 0xfa,0x044a, 0xfb,0x044b, 0xfc,0x044c, 0xfd,0x044d, 0xfe,0x044e, 0xff,0x044f,
   0,0
};

unsigned short glblCodePage866[] = {
   0x80,0x0410, 0x81,0x0411, 0x82,0x0412, 0x83,0x0413, 0x84,0x0414, 0x85,0x0415, 0x86,0x0416, 0x87,0x0417,
   0x88,0x0418, 0x89,0x0419, 0x8a,0x041a, 0x8b,0x041b, 0x8c,0x041c, 0x8d,0x041d, 0x8e,0x041e, 0x8f,0x041f,
   0x90,0x0420, 0x91,0x0421, 0x92,0x0422, 0x93,0x0423, 0x94,0x0424, 0x95,0x0425, 0x96,0x0426, 0x97,0x0427,
   0x98,0x0428, 0x99,0x0429, 0x9a,0x042a, 0x9b,0x042b, 0x9c,0x042c, 0x9d,0x042d, 0x9e,0x042e, 0x9f,0x042f,
   0xa0,0x0430, 0xa1,0x0431, 0xa2,0x0432, 0xa3,0x0433, 0xa4,0x0434, 0xa5,0x0435, 0xa6,0x0436, 0xa7,0x0437,
   0xa8,0x0438, 0xa9,0x0439, 0xaa,0x043a, 0xab,0x043b, 0xac,0x043c, 0xad,0x043d, 0xae,0x043e, 0xaf,0x043f,
   0xb0,0x2591, 0xb1,0x2592, 0xb2,0x2593, 0xb3,0x2502, 0xb4,0x2524, 0xb5,0x2561, 0xb6,0x2562, 0xb7,0x2556,
   0xb8,0x2555, 0xb9,0x2563, 0xba,0x2551, 0xbb,0x2557, 0xbc,0x255d, 0xbd,0x255c, 0xbe,0x255b, 0xbf,0x2510,
   0xc0,0x2514, 0xc1,0x2534, 0xc2,0x252c, 0xc3,0x251c, 0xc4,0x2500, 0xc5,0x253c, 0xc6,0x255e, 0xc7,0x255f,
   0xc8,0x255a, 0xc9,0x2554, 0xca,0x2569, 0xcb,0x2566, 0xcc,0x2560, 0xcd,0x2550, 0xce,0x256c, 0xcf,0x2567,
   0xd0,0x2568, 0xd1,0x2564, 0xd2,0x2565, 0xd3,0x2559, 0xd4,0x2558, 0xd5,0x2552, 0xd6,0x2553, 0xd7,0x256b,
   0xd8,0x256a, 0xd9,0x2518, 0xda,0x250c, 0xdb,0x2588, 0xdc,0x2584, 0xdd,0x258c, 0xde,0x2590, 0xdf,0x2580,
   0xe0,0x0440, 0xe1,0x0441, 0xe2,0x0442, 0xe3,0x0443, 0xe4,0x0444, 0xe5,0x0445, 0xe6,0x0446, 0xe7,0x0447,
   0xe8,0x0448, 0xe9,0x0449, 0xea,0x044a, 0xeb,0x044b, 0xec,0x044c, 0xed,0x044d, 0xee,0x044e, 0xef,0x044f,
   0xf0,0x0401, 0xf1,0x0451, 0xf2,0x0404, 0xf3,0x0454, 0xf4,0x0407, 0xf5,0x0457, 0xf6,0x040e, 0xf7,0x045e,
   0xf8,0x00b0, 0xf9,0x2219, 0xfa,0x00b7, 0xfb,0x221a, 0xfc,0x2116, 0xfd,0x00a4, 0xfe,0x25a0, 0xff,0x00a0,
   0,0
};

// :western
unsigned short glblCodePage1252[] = {
   0x80,0x20ac, 0x82,0x201a, 0x83,0x0192, 0x84,0x201e, 0x85,0x2026, 0x86,0x2020, 0x87,0x2021, 0x88,0x02c6,
   0x89,0x2030, 0x8a,0x0160, 0x8b,0x2039, 0x8c,0x0152, 0x8e,0x017d, 0x91,0x2018, 0x92,0x2019, 0x93,0x201c,
   0x94,0x201d, 0x95,0x2022, 0x96,0x2013, 0x97,0x2014, 0x98,0x02dc, 0x99,0x2122, 0x9a,0x0161, 0x9b,0x203a,
   0x9c,0x0153, 0x9e,0x017e, 0x9f,0x0178,
   0,0
};

unsigned short glblCodePage850[] = {
   0x80,0x00c7, 0x81,0x00fc, 0x82,0x00e9, 0x83,0x00e2, 0x84,0x00e4, 0x85,0x00e0, 0x86,0x00e5, 0x87,0x00e7,
   0x88,0x00ea, 0x89,0x00eb, 0x8a,0x00e8, 0x8b,0x00ef, 0x8c,0x00ee, 0x8d,0x00ec, 0x8e,0x00c4, 0x8f,0x00c5,
   0x90,0x00c9, 0x91,0x00e6, 0x92,0x00c6, 0x93,0x00f4, 0x94,0x00f6, 0x95,0x00f2, 0x96,0x00fb, 0x97,0x00f9,
   0x98,0x00ff, 0x99,0x00d6, 0x9a,0x00dc, 0x9b,0x00f8, 0x9c,0x00a3, 0x9d,0x00d8, 0x9e,0x00d7, 0x9f,0x0192,
   0xa0,0x00e1, 0xa1,0x00ed, 0xa2,0x00f3, 0xa3,0x00fa, 0xa4,0x00f1, 0xa5,0x00d1, 0xa6,0x00aa, 0xa7,0x00ba,
   0xa8,0x00bf, 0xa9,0x00ae, 0xaa,0x00ac, 0xab,0x00bd, 0xac,0x00bc, 0xad,0x00a1, 0xae,0x00ab, 0xaf,0x00bb,
   0xb0,0x2591, 0xb1,0x2592, 0xb2,0x2593, 0xb3,0x2502, 0xb4,0x2524, 0xb5,0x00c1, 0xb6,0x00c2, 0xb7,0x00c0,
   0xb8,0x00a9, 0xb9,0x2563, 0xba,0x2551, 0xbb,0x2557, 0xbc,0x255d, 0xbd,0x00a2, 0xbe,0x00a5, 0xbf,0x2510,
   0xc0,0x2514, 0xc1,0x2534, 0xc2,0x252c, 0xc3,0x251c, 0xc4,0x2500, 0xc5,0x253c, 0xc6,0x00e3, 0xc7,0x00c3,
   0xc8,0x255a, 0xc9,0x2554, 0xca,0x2569, 0xcb,0x2566, 0xcc,0x2560, 0xcd,0x2550, 0xce,0x256c, 0xcf,0x00a4,
   0xd0,0x00f0, 0xd1,0x00d0, 0xd2,0x00ca, 0xd3,0x00cb, 0xd4,0x00c8, 0xd5,0x0131, 0xd6,0x00cd, 0xd7,0x00ce,
   0xd8,0x00cf, 0xd9,0x2518, 0xda,0x250c, 0xdb,0x2588, 0xdc,0x2584, 0xdd,0x00a6, 0xde,0x00cc, 0xdf,0x2580,
   0xe0,0x00d3, 0xe1,0x00df, 0xe2,0x00d4, 0xe3,0x00d2, 0xe4,0x00f5, 0xe5,0x00d5, 0xe6,0x00b5, 0xe7,0x00fe,
   0xe8,0x00de, 0xe9,0x00da, 0xea,0x00db, 0xeb,0x00d9, 0xec,0x00fd, 0xed,0x00dd, 0xee,0x00af, 0xef,0x00b4,
   0xf0,0x00ad, 0xf1,0x00b1, 0xf2,0x2017, 0xf3,0x00be, 0xf4,0x00b6, 0xf5,0x00a7, 0xf6,0x00f7, 0xf7,0x00b8,
   0xf8,0x00b0, 0xf9,0x00a8, 0xfa,0x00b7, 0xfb,0x00b9, 0xfc,0x00b3, 0xfd,0x00b2, 0xfe,0x25a0, 0xff,0x00a0,
   0,0
};

SFKChars sfkchars;

int setsfkcharsacp(int i)
{
   return sfkchars.setacp(i);
}

SFKChars::SFKChars( )
{
   memset(this, 0, sizeof(*this));

   #ifndef _WIN32
   iclocp = 850;  // linux/mac default
   iclacp = 1252; // linux/mac default
   #endif
}

/*
   even if the system has codepage 1252 the fixed glblCodePage1252
   is used internally, assuring the same behaviour on all computers
   in case that -isochars is used.
*/
ushort SFKChars::ibytetouni(uchar c, ushort icp)
{
   ushort *pfixedcp=0;
   bool bdos=0;

   switch (icp)
   {
      case 1250: pfixedcp = glblCodePage1250; break;
      case  852: pfixedcp = glblCodePage852;  bdos=1; break;
      case 1251: pfixedcp = glblCodePage1251; break;
      case  866: pfixedcp = glblCodePage866;  bdos=1; break;
      case 1252: pfixedcp = glblCodePage1252; break;
      case  850: pfixedcp = glblCodePage850;  bdos=1; break;
   }
   if (pfixedcp) {
      for (int i=0; pfixedcp[i]; i+=2)
         if (pfixedcp[i+0]==c)
            return pfixedcp[i+1];
      // Ansi: everything else maps 1:1
      // Dos : everything else >= 0x80 does not exist
      return c;
   }

   #ifdef _WIN32

   sztmp[0] = c;
   sztmp[1] = 0;

   awtmp[0] = 0;

   MultiByteToWideChar( // win.varcp
      icp, 0, sztmp, 1,
      (wchar_t*)awtmp, 10
      );

   ushort ntmp = awtmp[0];

   if (ntmp)
      return ntmp;

   #endif

   return 0;
}

int SFKChars::setocp(ushort i)
{
   switch (i)
   {
      // support for unzip -fromcode:
      case 1250: case 1251: case 1252:
         if (!banycp)
            return 9;
         // else fall through
      #ifdef _WIN32
      default:
      #endif
      case 852: case 866: case 850:
         iclocp=i;
         bsysocp=0;
         bclinited=0;
         return 0;
   }
   return 9;
}

int SFKChars::setacp(ushort i)
{
   switch (i)
   {
      #ifdef _WIN32
      default:
      #endif
      case 1250: case 1251: case 1252:
         iclacp=i;
         bsysacp=0;
         bclinited=0;
         return 0;
   }
   return 9;
}

ushort SFKChars::getocp( )
   { init(); return iclocp; }

ushort SFKChars::getacp( )
   { init(); return iclacp; }

int SFKChars::init( )
{
   if (bclinited)
      return 0;

   ushort n1=iclocp;
   ushort n2=iclacp;
    memset(this, 0, sizeof(*this));
   iclocp=n1;
   iclacp=n2;
   bclinited = 1;

   #ifdef _WIN32

   if (iclocp==0) {
      iclocp = GetOEMCP();
      bsysocp = 1;
   }

   if (iclacp==0) {
      iclacp = GetACP();
      bsysacp = 1;
   }

   #endif

   ushort coem=0,cans=0,cuni=0;
   ushort noemmaps=0,nansmaps=0;

   // build byte to uni mapping
   for (ushort i=0; i<256; i++)
   {
      coem = i;
      cuni = ibytetouni(coem, iclocp);
      amap1[coem] = cuni;
      amap2[cuni] = coem;
      if (i >= 0x80 && cuni != 0)
         noemmaps++;

      cans = i;
      cuni = ibytetouni(cans, iclacp);
      amap3[cans] = cuni;
      amap4[cuni] = cans;
      if (i >= 0x80 && cuni != 0)
         nansmaps++;
   }

   if (noemmaps < 1) return 10;
   if (nansmaps < 1) return 11;

   // build byte crossmapping
   for (ushort i=0; i<256; i++)
   {
      coem = i;
      cuni = oemtouni(coem);
      cans = unitoansi(cuni);
      amap5[coem] = cans ? cans : '?';
      if (!cans)
         amap5err[coem] = 1;

      cans = i;
      cuni = ansitouni(cans);
      coem = unitooem(cuni);
      amap6[cans] = coem ? coem : '?';
      if (!coem)
         amap6err[cans] = 1;
   }

   return 0;
}

ushort SFKChars::ansitouni(uchar  c)
   { init(); return amap3[c]; }

ushort SFKChars::oemtouni(uchar  c)
   { init(); return amap1[c]; }

uchar SFKChars::unitoansi(ushort n)
   { init(); return amap4[n]; }

uchar SFKChars::unitooem(ushort n)
   { init(); return amap2[n]; }

uchar SFKChars::oemtoansi(uchar  c)
   { init(); return amap5[c]; }

uchar SFKChars::ansitooem(uchar  c)
   { init(); return amap6[c]; }

int SFKChars::stroemtoansi(char *psz, int *pChg, bool bNoDefault)
{
   int ifailed=0;
   init();
   for (; *psz; psz++)
   {
      if (amap5err[(uchar)*psz]) {
         ifailed++;
         if (bNoDefault)
            continue;
      }
      char c = amap5[(uchar)*psz];
      if (pChg!=0 && *psz!=c)
         *pChg++;
      *psz = c;
   }
   return ifailed;
}

int SFKChars::stransitooem(char *psz, int *pChg, bool bNoDefault)
{
   int ifailed=0;
   init();
   for (; *psz; psz++)
   {
      if (amap6err[(uchar)*psz]) {
         ifailed++;
         if (bNoDefault)
            continue;
      }
      char c = (char)amap6[(uchar)*psz];
      if (pChg!=0 && *psz!=c)
         *pChg++;
      *psz = c;
   }
   return ifailed;
}

int SFKChars::strunitoansi(ushort *puni, int iunilen,
   char *pansi, int imaxansi)
{
   if (imaxansi < 1)
      return 10;

   init();

   int irc = 0;

   ushort *punicur = puni;
   ushort *punimax = puni+iunilen;
   uchar  *panscur = (uchar*)pansi;
   uchar  *pansmax = panscur+imaxansi;

   while (punicur<punimax && panscur+4<pansmax)
   {
      ushort nuni = *punicur++;
      uchar  cans = unitoansi(nuni);
      if (!cans)
         { cans = '?'; irc = 9; }
      *panscur++ = cans;
   }

   *panscur = '\0';

   return irc;
}

int SFKChars::wlen(ushort *puni)
{
   ushort *pcur=puni;
   while (*pcur) pcur++;
   return (int)(pcur - puni);
}

num getAnsiToUTFSize(char *psrc)
{
   char szTmp[100];

   num nout = 0;

   while (*psrc!=0)
   {
      uchar cans  = *psrc++;
      ushort nuni = sfkchars.ansitouni(cans);
      nout += UTF8Codec::toutf8((char*)szTmp, 10, nuni);
   }

   return nout;
}

int ansiToUTF(char *pdst, int imaxdst, char *psrc)
{
   char *pdstcur=pdst;
   char *pdstmax=pdst+imaxdst;
   while (*psrc!=0 && pdstcur+10<pdstmax)
   {
      uchar cans  = *psrc++;
      ushort nuni = sfkchars.ansitouni(cans);
      int iwrite  = UTF8Codec::toutf8((char*)pdstcur, 10, nuni);
      pdstcur += iwrite;
   }
   *pdstcur = '\0';
   return 0;
}

void utfToAnsi(char *pdst, int imaxdst, char *psrc)
{
   UTF8Codec utf(psrc);

   char *pdstcur = pdst;
   char *pdstmax = pdst+imaxdst;

   while (pdstcur+10<pdstmax)
   {
      ushort nuni = utf.nextChar();
      if (nuni == 0)
         break;
      uchar cans = sfkchars.unitoansi(nuni);
      *pdstcur++ = cans ? cans : '?';
   }
   *pdstcur = '\0';
}

char *ansiToUTF(char *psrc) {
   static char aaBuf[3][MAX_LINE_LEN+10];
   static int  iaBuf=0;
   iaBuf = (iaBuf+1)%3;
   ansiToUTF(aaBuf[iaBuf],MAX_LINE_LEN,psrc);
   return aaBuf[iaBuf];
}
char *utfToAnsi(char *psrc) {
   static char aaBuf[3][MAX_LINE_LEN+10];
   static int  iaBuf=0;
   iaBuf = (iaBuf+1)%3;
   utfToAnsi(aaBuf[iaBuf],MAX_LINE_LEN,psrc);
   return aaBuf[iaBuf];
}

/*
   -  so far windows dependent,
      linux maps only basic latin chars

   -  unicode mapping of many special chars
*/

extern unsigned short atol1to1[];
extern unsigned short atolfuzz[];

SFKNoCase sfknocasesharp(0);
SFKNoCase sfknocasefuzz(1);

SFKNoCase::SFKNoCase(bool bfuzz)
{
   memset(this,0,sizeof(*this));

   bclfuzz = bfuzz;
}

void SFKNoCase::tellPage( )
{
   if (bcltoldcp)
      return;
   bcltoldcp = 1;
   sfkchars.init();
   printf("[nocase : using %s Ansi page %u for mapping, %s OEM page %u for display]\n",
      sfkchars.bsysacp ? "system":"fixed", sfkchars.iclacp,
      sfkchars.bsysocp ? "system":"fixed", sfkchars.iclocp
      );
}

char *getuniname(ushort ncode);



uchar SFKNoCase::itolower(uchar c)
{
   // cmod text_uni
   #if (sfk_prog || sfk_text_uni)

   #ifdef _WIN32

   if (cs.nocasemin)
      return tolower(c);

   ushort auni[2];

   if (atolower[c])
      return atolower[c];

   atolower[c] = map1to1(c, 1, auni);

   if (atolower[c] != c)
      aisalpha[c] = 1;
   else if (auni[0] != 0 && auni[0] == auni[1])
      aisalpha[c] = 1; // e.g. 01A6;LATIN LETTER YR

   if (cs.tracecase)
   {
      tellPage();
      char *pname1 = getuniname(auni[0]);
      char *pname2 = getuniname(auni[1]);
      if (atolower[c] != c)
         oprintf("[tolower: Ansi %c 0x%02x U+%04x -> %c 0x%02x U+%04x - %s]\n",
            c, c, auni[0],
            atolower[c], atolower[c], auni[1], pname1);
      else if (cs.tracecase > 1)
         oprintf("[tolower: Ansi %c 0x%02x U+%04x -- no lo mapping - %s]\n",
            c, c, auni[0], pname1);
   }

   return atolower[c];

   #else

   return tolower(c);

   #endif

   #else

   return tolower(c);

   // emod text_uni
   #endif // (sfk_prog || sfk_text_uni)
}

uchar SFKNoCase::itoupper(uchar c)
{
   // cmod text_uni
   #if (sfk_prog || sfk_text_uni)

   #ifdef _WIN32

   if (cs.nocasemin)
      return toupper(c);

   ushort auni[2];

   if (atoupper[c])
      return atoupper[c];

   atoupper[c] = map1to1(c, 0, auni);

   if (atoupper[c] != c)
      aisalpha[c] = 1;
   else if (auni[0] != 0 && auni[0] == auni[1])
      aisalpha[c] = 1; // e.g. 01A6;LATIN LETTER YR

   if (cs.tracecase)
   {
      tellPage();
      char *pname1 = getuniname(auni[0]);
      char *pname2 = getuniname(auni[1]);
      if (atoupper[c] != c)
         oprintf("[toupper: Ansi %c 0x%02x U+%04x -> %c 0x%02x U+%04x - %s]\n",
            c, c, auni[0],
            atoupper[c], atoupper[c], auni[1], pname1);
      else if (cs.tracecase > 1)
         oprintf("[toupper: Ansi %c 0x%02x U+%04x -- no up mapping - %s]\n",
            c, c, auni[0], pname1);
   }

   return atoupper[c];

   #else

   return toupper(c); // FIX sfk193 linux case insensitivity

   #endif

   #else

   return toupper(c);

   // emod text_uni
   #endif // (sfk_prog || sfk_text_uni)
}

void SFKNoCase::isetStringToLower(char *psz)
{
   for (int i=0; psz[i]; i++)
   {
      psz[i] = itolower((uchar)psz[i]);
   }
}

bool SFKNoCase::iisalpha(uchar c)
{
   if (aisalpha[c] == 0) // not yet checked
   {
      if (isalpha(c))
         aisalpha[c] = 1;
      else {
         if (itolower(c) != c)
            aisalpha[c] = 1;
         else
         if (itoupper(c) != c)
            aisalpha[c] = 1;
         else
            aisalpha[c] = 2;
      }
   }
   if (aisalpha[c] == 1)
      return 1;
   return 0;
}

bool SFKNoCase::iisalnum(uchar c)
{
   if (iisalpha(c))
      return 1;
   if (isdigit(c))
      return 1;
   return 0;
}

bool SFKNoCase::iisprint(uchar uc)
{
   if (iisalpha(uc))
      return 1;
   if (isdigit(uc))
      return 1;
   // linux: isprint(':') returns 16384
   // so never return the value directly.
   if (isprint((char)uc))
      return 1;
   return 0;
}

int sfktolower(int c)
{
   // nocase search uses tolower for matching,
   // so -deacc is applied.
   if (cs.fuzz)
      return sfknocasefuzz.itolower(c);
   else
      return sfknocasesharp.itolower(c);
}

void sfkSetStringToLower(char *psz)
{
   if (cs.fuzz)
      return sfknocasefuzz.isetStringToLower(psz);
   else
      return sfknocasesharp.isetStringToLower(psz);
}

uchar sfkMapChar(char ch, uchar bCase)
{
   if (cs.fuzz)
      return sfknocasefuzz.mapChar(ch, bCase);
   else
      return sfknocasesharp.mapChar(ch, bCase);
}

uchar sfkLowerUChar(uchar c)
{
   if (cs.fuzz)
      return sfknocasefuzz.lowerUChar(c);
   else
      return sfknocasesharp.lowerUChar(c);
}

uchar sfkUpperUChar(uchar c)
{
   return sfknocasesharp.lowerUChar(c);
}

// set all possible matches for this character
void sfkSetHeadMatch(uchar ucFirst, uchar aHeadMatch[])
{
   if (cs.fuzz == 0)
   {
      // normally there is a 1:1 mapping of lower- and upper char.
      aHeadMatch[sfktoupper(ucFirst)&0xFF] = 1;
      aHeadMatch[sfktolower(ucFirst)&0xFF] = 1;
      return;
   }

   // with -deacc many codes may map to a single char.
   // user types a, it can match a A a_accent A_accent etc.
   ucFirst = sfktolower(ucFirst);
   aHeadMatch[ucFirst] = 1;
   for (ushort n=1; n<256; n++)
   {
      if (sfktolower(n) == ucFirst) aHeadMatch[n] = 1;
      // if (sfktoupper(n) == ucFirst) aHeadMatch[n] = 1;
   }
}

int sfktoupper(int c)
{
   // toupper is never used in nocase comparisons,
   // so -deacc does not apply.
   return sfknocasesharp.itoupper(c);
}

bool sfkisalpha(uchar uc)
{
   if (cs.nocasemin)
      return isalpha((char)uc) ? 1 : 0;
      // converts int value eg. 16384 to bool!

   return sfknocasesharp.iisalpha(uc);
}

bool sfkisalnum(uchar uc)
{
   if (cs.nocasemin)
      return isalnum((char)uc) ? 1 : 0;
      // converts int value eg. 16384 to bool!

   return sfknocasesharp.iisalnum(uc);
}

bool sfkisprint(uchar uc)
{
   if (cs.nocasemin)
      return isprint((char)uc) ? 1 : 0;
      // converts int value eg. 16384 to bool!

   return sfknocasesharp.iisprint(uc);
}

// void sfkSetAccentSupport(bool b) // sfk190 deprecated
// {
//    gs.noaccent = cs.noaccent = b ? 0 : 1;
// }

// --- nocase derived functions ---

#ifdef WITH_CASE_XNN
// RC : 0 == match, <> 0 == no match.
// Not suitable for sorting algorithms.
int sfkmemcmp2(uchar *psrc1, uchar *psrc2, num nlen, bool bGlobalCase, uchar *pFlags)
{
   if (bGlobalCase)
      return memcmp(psrc1, psrc2, nlen);

   int idiff=0;

   // optim: compare last character first.
   // requires at least a 2-char phrase.
   if (nlen > 1)
   {
      uchar bCase = pFlags ? sfkGetBit(pFlags,nlen-1) : 0;
      idiff =     sfkMapChar(psrc1[nlen-1],bCase)
               -  sfkMapChar(psrc2[nlen-1],bCase);
      if (idiff)
         return idiff;
   }

   uchar bCase;

   for (int i=0; i<nlen; i++)
   {
      bCase = pFlags ? sfkGetBit(pFlags,i) : 0;
      idiff =     sfkMapChar(psrc1[i],bCase)
               -  sfkMapChar(psrc2[i],bCase);
      if (idiff)
         break;
   }

   return idiff;
}
#endif

// RC : 0 == match, <> 0 == no match.
// Not suitable for sorting algorithms.
int sfkmemcmp3(uchar *psrc, uchar *ppat, num nlen, bool bGlobalCase, uchar *pPatFlags, int iPatOff)
{
   ppat += iPatOff;

   if (bGlobalCase)
      return memcmp(psrc, ppat, nlen);

   int idiff=0;

   // optim: compare last character first.
   // requires at least a 2-char phrase.
   if (nlen > 1)
   {
      uchar bCase = pPatFlags ? sfkGetBit(pPatFlags,nlen+iPatOff-1) : 0;
      idiff =     sfkMapChar(psrc[nlen-1],bCase)
               -  sfkMapChar(ppat[nlen-1],bCase);
      if (idiff)
         return idiff;
   }

   uchar bCase;

   for (int i=0; i<nlen; i++)
   {
      bCase = pPatFlags ? sfkGetBit(pPatFlags,iPatOff+i) : 0;
      idiff =     sfkMapChar(psrc[i],bCase)
               -  sfkMapChar(ppat[i],bCase);
      if (idiff)
         break;
   }

   return idiff;
}

// RC : 0 == match, <> 0 == no match.
// Not suitable for sorting algorithms.
int sfkmemcmp(uchar *psrc1, uchar *psrc2, num nlen, bool bcase)
{
   if (bcase)
      return memcmp(psrc1, psrc2, nlen);

   num i=0, idiff=0;

   // optim: compare last character first.
   // requires at least a 2-char phrase.
   if (nlen > 1)
   {
      idiff =     sfkLowerUChar(psrc1[nlen-1])
               -  sfkLowerUChar(psrc2[nlen-1]);
      if (idiff)
         return idiff;
   }

   for (; i<nlen; i++)
   {
      idiff =     sfkLowerUChar(psrc1[i])
               -  sfkLowerUChar(psrc2[i]);

      if (idiff)
         break;
   }

   return idiff;
}

#ifdef WITH_CASE_XNN
uchar *memIFind(uchar *pNeedle, num nNeedleSize, uchar *pHayStack, num nHaySize, uchar *pFlags)
{
   uchar *pCur = pHayStack;
   uchar *pMax = pHayStack + nHaySize - nNeedleSize; // inclusive
   uchar bCase = pFlags ? sfkGetBit(pFlags,0) : 0;
   uchar c1    = sfkMapChar(*pNeedle,bCase);
   while (pCur <= pMax)
   {
      uchar *p1 = pCur;
      // seek to next potential start
      while ((p1 <= pMax) && (sfkMapChar(*p1,bCase) != c1))
         p1++;
      if (!p1 || (p1 > pMax))
         return 0;
      // compare from p1, case-insensitive
      // pMax assures that from p1, there are at least nNeedleSize
      // bytes available for actual compare, so we don't check
      // p1 against pMax again here.
      num i=0;
      for (; i<nNeedleSize; i++) {
         bCase = pFlags ? sfkGetBit(pFlags,i) : 0;
         if (sfkMapChar(pNeedle[i],bCase) != sfkMapChar(p1[i],bCase))
            break;
      }
      if (i >= nNeedleSize)
         return p1;  // hit
      // no hit, proceed
      pCur = p1+1;
   }
   return 0;
}
#else
uchar *memIFind(uchar *pNeedle, num nNeedleSize, uchar *pHayStack, num nHaySize)
{
   uchar *pCur = pHayStack;
   uchar *pMax = pHayStack + nHaySize - nNeedleSize; // inclusive
   uchar c1    = sfkLowerUChar(*pNeedle);
   while (pCur <= pMax)
   {
      uchar *p1 = pCur;
      // seek to next potential start
      while ((p1 <= pMax) && (sfkLowerUChar(*p1) != c1))
         p1++;
      if (!p1 || (p1 > pMax))
         return 0;
      // compare from p1, case-insensitive
      // pMax assures that from p1, there are at least nNeedleSize
      // bytes available for actual compare, so we don't check
      // p1 against pMax again here.
      num i=0;
      for (; i<nNeedleSize; i++)
         if (sfkLowerUChar(pNeedle[i]) != sfkLowerUChar(p1[i]))
            break;
      if (i >= nNeedleSize)
         return p1;  // hit
      // no hit, proceed
      pCur = p1+1;
   }
   return 0;
}
#endif

// paged output with oprintf
void stepMore()
{
   if (!cs.nmore)
      return;

   if (cs.imore+1 >= cs.nmore)
   {
      cs.morepage++;
      setTextColor(nGlblTimeColor, 1);
      printf("--- %03u [enter/c/x] ---\r", cs.morepage); fflush(stdout);
      int c = getchar();
      setTextColor(-1);
      cs.imore = 0;
      if (c == 'c')
         cs.nmore = 0;
      if (c == 'x' || c == 'q')
         bGlblEscape = 1;
   }
}

// printf with OEM output conversion

void oprintf(cchar *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBufMap, sizeof(szPrintBufMap)-10, pszFormat, argList);
   szPrintBufMap[sizeof(szPrintBufMap)-10] = '\0';

   #ifdef _WIN32
   char *psz = szPrintBufMap;
   // windows only: if output is NOT directed to file, map it to DOS charset,
   // to have filenames listed with correct umlauts etc.
   if (cs.outcconv && (cs.forcecconv || bGlblHaveInteractiveConsole))
   {
      sfkchars.stransitooem(psz);
   }
   #endif

   printf("%s", szPrintBufMap);
   fflush(stdout);

   if (cs.nmore) {
      for (int i=0; szPrintBufMap[i]; i++)
         if (szPrintBufMap[i]=='\n')
            cs.imore++;
      if (strEndsWith(szPrintBufMap, '\n'))
         stepMore();
   }
}

void oprintf(StringPipe *pOutData, cchar *pszFormat, ...)
{
   va_list argList;
   va_start(argList, pszFormat);
   ::vsnprintf(szPrintBufMap, sizeof(szPrintBufMap)-10, pszFormat, argList);
   szPrintBufMap[sizeof(szPrintBufMap)-10] = '\0';

   char *psz = szPrintBufMap;

   if (pOutData) {
      // auto-strip LF if given. oprintf adds full records only.
      // do NOT convert text, as it is passed to further chain commands.
      int nlen = strlen(psz);
      if (nlen > 0 && psz[nlen-1] == '\n')
         psz[nlen-1] = '\0';
      pOutData->addEntry(psz, str(""));
   } else {
      #ifdef _WIN32
      // windows only: if output is NOT directed to file, map it to DOS charset,
      // to have filenames listed with correct umlauts etc.
      if (cs.outcconv && (cs.forcecconv || bGlblHaveInteractiveConsole))
      {
         sfkchars.stransitooem(psz);
      }
      #endif
      printf("%s", szPrintBufMap);
      fflush(stdout);
      if (cs.nmore) {
         for (int i=0; szPrintBufMap[i]; i++)
            if (szPrintBufMap[i]=='\n')
               cs.imore++;
         if (strEndsWith(szPrintBufMap, '\n'))
            stepMore();
      }
   }
}

// - - - sfk md5

SFKMD5::SFKMD5() { reset(); }

void SFKMD5::reset()
{
   bClDigDone  = 0;

   nClDigLen   = 16;
   nClBufLen   = 64;
   pClBuf      = aClBuf;
   pClDig      = aClDig;

   nClCntHigh  = nClCntLow = nClBufCnt = 0;

   alClSta[0] = 0x67452301;
   alClSta[1] = 0xefcdab89;
   alClSta[2] = 0x98badcfe;
   alClSta[3] = 0x10325476;

   nClCRC = 0;
}

SFKMD5::~SFKMD5() { }

void SFKMD5::update(uchar *pData, uint32_t nLen)
{
   #ifdef SFKPACK
   if (cs.crcmd5) {
      nClCRC = sfkPackSum(pData, nLen, nClCRC);
      return;
   }
   #endif // SFKPACK

   if (bGlblOldMD5)
   {
      for (uint32_t i=0; i<nLen; i++)
         update(pData[i]);
   }
   else
   {
      int nCnt = 0;
 
      if (nClBufCnt != 0)
      {
         int nAvail = nClBufLen - nClBufCnt;
         int nMax = nAvail < (int)nLen ? nAvail : (int)nLen;
 
         for (; nCnt < nMax; nCnt++)
            update(pData[nCnt]);
      }
 
      int nBlocks = (nLen - nCnt) / nClBufLen;
 
      unsigned int ulNumChars = nBlocks * nClBufLen;
 
      nClCntLow += ulNumChars;
 
      if (nClCntLow < ulNumChars)
         nClCntHigh++;
 
      for (int i = 0; i < nBlocks; i++)
      {
         memcpy(pClBuf, pData + nCnt, nClBufLen);
         transform();
         nCnt += nClBufLen;
      }
 
      for (; nCnt < (int)nLen; nCnt++)
         update(pData[nCnt]);
   }
}

void SFKMD5::update(uchar c)
{
   pClBuf[nClBufCnt++] = c;
   if (nClBufCnt == nClBufLen)
   {
      transform();
      nClBufCnt = 0;
   }
   if (++nClCntLow == 0)
      nClCntHigh++;
}

void SFKMD5::transform()
{
   #define MD5XOR0(x, y, z) (z ^ (x & (y ^ z)))
   #define MD5XOR1(x, y, z) (y ^ (z & (x ^ y)))
   #define MD5XOR2(x, y, z) (x ^ y ^ z)
   #define MD5XOR3(x, y, z) (y  ^  (x | ~z))
   #define MD5ROT(a, n) a = (a << n) | (a >> (32 - n))
   #define MD5PR0(a, b, c, d, k, s, t) { a += k + t + MD5XOR0(b, c, d); MD5ROT(a, s); a += b; }
   #define MD5PR1(a, b, c, d, k, s, t) { a += k + t + MD5XOR1(b, c, d); MD5ROT(a, s); a += b; }
   #define MD5PR2(a, b, c, d, k, s, t) { a += k + t + MD5XOR2(b, c, d); MD5ROT(a, s); a += b; }
   #define MD5PR3(a, b, c, d, k, s, t) { a += k + t + MD5XOR3(b, c, d); MD5ROT(a, s); a += b; }

   uchar *pBuf = pClBuf;
   for (uint32_t i = 0; i < 16; i++)
   {
      uint32_t ul;
      ul = pBuf[0];
      ul |= pBuf[1] << 8;
      ul |= pBuf[2] << 16;
      ul |= pBuf[3] << 24;
      pBuf += 4;
      alClBuf[i] = ul;
   }

   uint32_t a = alClSta[0];
   uint32_t b = alClSta[1];
   uint32_t c = alClSta[2];
   uint32_t d = alClSta[3];

   MD5PR0(a, b, c, d, alClBuf[ 0],  7, 0xd76aa478);
   MD5PR0(d, a, b, c, alClBuf[ 1], 12, 0xe8c7b756);
   MD5PR0(c, d, a, b, alClBuf[ 2], 17, 0x242070db);
   MD5PR0(b, c, d, a, alClBuf[ 3], 22, 0xc1bdceee);
   MD5PR0(a, b, c, d, alClBuf[ 4],  7, 0xf57c0faf);
   MD5PR0(d, a, b, c, alClBuf[ 5], 12, 0x4787c62a);
   MD5PR0(c, d, a, b, alClBuf[ 6], 17, 0xa8304613);
   MD5PR0(b, c, d, a, alClBuf[ 7], 22, 0xfd469501);
   MD5PR0(a, b, c, d, alClBuf[ 8],  7, 0x698098d8);
   MD5PR0(d, a, b, c, alClBuf[ 9], 12, 0x8b44f7af);
   MD5PR0(c, d, a, b, alClBuf[10], 17, 0xffff5bb1);
   MD5PR0(b, c, d, a, alClBuf[11], 22, 0x895cd7be);
   MD5PR0(a, b, c, d, alClBuf[12],  7, 0x6b901122);
   MD5PR0(d, a, b, c, alClBuf[13], 12, 0xfd987193);
   MD5PR0(c, d, a, b, alClBuf[14], 17, 0xa679438e);
   MD5PR0(b, c, d, a, alClBuf[15], 22, 0x49b40821);

   MD5PR1(a, b, c, d, alClBuf[ 1],  5, 0xf61e2562);
   MD5PR1(d, a, b, c, alClBuf[ 6],  9, 0xc040b340);
   MD5PR1(c, d, a, b, alClBuf[11], 14, 0x265e5a51);
   MD5PR1(b, c, d, a, alClBuf[ 0], 20, 0xe9b6c7aa);
   MD5PR1(a, b, c, d, alClBuf[ 5],  5, 0xd62f105d);
   MD5PR1(d, a, b, c, alClBuf[10],  9, 0x02441453);
   MD5PR1(c, d, a, b, alClBuf[15], 14, 0xd8a1e681);
   MD5PR1(b, c, d, a, alClBuf[ 4], 20, 0xe7d3fbc8);
   MD5PR1(a, b, c, d, alClBuf[ 9],  5, 0x21e1cde6);
   MD5PR1(d, a, b, c, alClBuf[14],  9, 0xc33707d6);
   MD5PR1(c, d, a, b, alClBuf[ 3], 14, 0xf4d50d87);
   MD5PR1(b, c, d, a, alClBuf[ 8], 20, 0x455a14ed);
   MD5PR1(a, b, c, d, alClBuf[13],  5, 0xa9e3e905);
   MD5PR1(d, a, b, c, alClBuf[ 2],  9, 0xfcefa3f8);
   MD5PR1(c, d, a, b, alClBuf[ 7], 14, 0x676f02d9);
   MD5PR1(b, c, d, a, alClBuf[12], 20, 0x8d2a4c8a);

   MD5PR2(a, b, c, d, alClBuf[ 5],  4, 0xfffa3942);
   MD5PR2(d, a, b, c, alClBuf[ 8], 11, 0x8771f681);
   MD5PR2(c, d, a, b, alClBuf[11], 16, 0x6d9d6122);
   MD5PR2(b, c, d, a, alClBuf[14], 23, 0xfde5380c);
   MD5PR2(a, b, c, d, alClBuf[ 1],  4, 0xa4beea44);
   MD5PR2(d, a, b, c, alClBuf[ 4], 11, 0x4bdecfa9);
   MD5PR2(c, d, a, b, alClBuf[ 7], 16, 0xf6bb4b60);
   MD5PR2(b, c, d, a, alClBuf[10], 23, 0xbebfbc70);
   MD5PR2(a, b, c, d, alClBuf[13],  4, 0x289b7ec6);
   MD5PR2(d, a, b, c, alClBuf[ 0], 11, 0xeaa127fa);
   MD5PR2(c, d, a, b, alClBuf[ 3], 16, 0xd4ef3085);
   MD5PR2(b, c, d, a, alClBuf[ 6], 23, 0x04881d05);
   MD5PR2(a, b, c, d, alClBuf[ 9],  4, 0xd9d4d039);
   MD5PR2(d, a, b, c, alClBuf[12], 11, 0xe6db99e5);
   MD5PR2(c, d, a, b, alClBuf[15], 16, 0x1fa27cf8);
   MD5PR2(b, c, d, a, alClBuf[ 2], 23, 0xc4ac5665);

   MD5PR3(a, b, c, d, alClBuf[ 0],  6, 0xf4292244);
   MD5PR3(d, a, b, c, alClBuf[ 7], 10, 0x432aff97);
   MD5PR3(c, d, a, b, alClBuf[14], 15, 0xab9423a7);
   MD5PR3(b, c, d, a, alClBuf[ 5], 21, 0xfc93a039);
   MD5PR3(a, b, c, d, alClBuf[12],  6, 0x655b59c3);
   MD5PR3(d, a, b, c, alClBuf[ 3], 10, 0x8f0ccc92);
   MD5PR3(c, d, a, b, alClBuf[10], 15, 0xffeff47d);
   MD5PR3(b, c, d, a, alClBuf[ 1], 21, 0x85845dd1);
   MD5PR3(a, b, c, d, alClBuf[ 8],  6, 0x6fa87e4f);
   MD5PR3(d, a, b, c, alClBuf[15], 10, 0xfe2ce6e0);
   MD5PR3(c, d, a, b, alClBuf[ 6], 15, 0xa3014314);
   MD5PR3(b, c, d, a, alClBuf[13], 21, 0x4e0811a1);
   MD5PR3(a, b, c, d, alClBuf[ 4],  6, 0xf7537e82);
   MD5PR3(d, a, b, c, alClBuf[11], 10, 0xbd3af235);
   MD5PR3(c, d, a, b, alClBuf[ 2], 15, 0x2ad7d2bb);
   MD5PR3(b, c, d, a, alClBuf[ 9], 21, 0xeb86d391);

   alClSta[0] += a;
   alClSta[1] += b;
   alClSta[2] += c;
   alClSta[3] += d;
}

uchar *SFKMD5::digest()
{
   #ifdef SFKPACK
   if (cs.crcmd5) {
      memset(aClDig, 0, 16);
      aClDig[0] = ((nClCRC >> 24) & 0xFFU);
      aClDig[1] = ((nClCRC >> 16) & 0xFFU);
      aClDig[2] = ((nClCRC >>  8) & 0xFFU);
      aClDig[3] = ((nClCRC >>  0) & 0xFFU);
      return pClDig;
   }
   #endif // SFKPACK

   if (bClDigDone)
      return pClDig;

   uint32_t lLow  = nClCntLow << 3;
   uint32_t lHigh = (nClCntLow >> 29) | (nClCntHigh << 3);

   update(128);

   while ((nClCntLow & 63) != 56)
      update(0);

   update((uchar)lLow);
   update((uchar)(lLow >> 8));
   update((uchar)(lLow >> 16));
   update((uchar)(lLow >> 24));

   update((uchar)lHigh);
   update((uchar)(lHigh >> 8));
   update((uchar)(lHigh >> 16));
   update((uchar)(lHigh >> 24));

   // above code should ensure that last update() lead to empty buffer.
   if (nClBufCnt != 0) { fprintf(stderr, "ERROR: SFKMD5 internal #1\n"); }

   uchar *pDigest = pClDig;
   for (uint32_t i = 0; i < 4; i++)
   {
      uint32_t ul = alClSta[i];
      pDigest[0] = (uchar)ul; ul >>= 8;
      pDigest[1] = (uchar)ul; ul >>= 8;
      pDigest[2] = (uchar)ul; ul >>= 8;
      pDigest[3] = (uchar)ul;
      pDigest += 4;
   }

   bClDigDone = 1;
   return pClDig;
}

// - - - sfk home and tmp folder

SFKHome sfkhome;

SFKHome::SFKHome( )
{
   mclear(szClDir);
   bClTold = 0;

   #ifdef _WIN32

   char *psz = getenv("SFK_HOME");
   if (psz) {
      strcopy(szClDir, psz);
      return;
   }

   psz = getenv("LOCALAPPDATA");
   if (psz) {
      snprintf(szClDir, sizeof(szClDir)-10, "%s\\.sfkhome", psz);
      return;
   }

   // strcpy(szClDir, "\\.sfkhome");

   #else

   // FIX: 174: wrong sfkhome under linux
   // sprintf(szClDir, "~/.sfkhome");

   char *psz = getenv("SFK_HOME");
   if (psz) {
      strcopy(szClDir, psz);
      return;
   }

   psz = getenv("HOME");
   if (psz) {
      snprintf(szClDir, sizeof(szClDir)-10, "%s/.sfkhome", psz);
      return;
   }

   do
   {
      struct passwd *ppw = getpwuid(getuid());
      if (!ppw)
         break;
      const char *pszhome = ppw->pw_dir;
      if (!pszhome)
         break;
      snprintf(szClDir, sizeof(szClDir)-10, "%s/.sfkhome", pszhome);
      return;
   }
   while (0);

   // strcpy(szClDir, "/.sfkhome");

   #endif
}

bool SFKHome::noHomeDir()
{
   if (szClDir[0])
      return 0;

   if (!bClTold)
   {
      bClTold = 1;

      pwarn("no SFK Home Dir exists to store or read data.");
 
      #ifdef _WIN32
      pinf("you may SET \"SFK_HOME=anyfolder\" to define it directly.\n");
      pinf("you may SET \"LOCALAPPDATA=anyfolder\" to define it's parent folder.\n");
      #endif
   }

   return 1;
}

char *SFKHome::makePath(char *pszRelPath, bool bReadOnly)
{
   if (noHomeDir())
      return 0;

   snprintf(szClPathBuf, sizeof(szClPathBuf)-10,
      "%s%c%s", szClDir, glblPathChar, pszRelPath);

   if (!bReadOnly && createOutDirTree(szClPathBuf))
      return 0;

   return szClPathBuf;
}

char *SFKHome::getPath(char *pszRelPath)
{
   return makePath(pszRelPath, 1);
}

int SFTmpFile::ncnt = 1;
char *SFTmpFile::pszTmpDir = 0;

SFTmpFile::SFTmpFile(const char *pszExt, bool bNoAutoDelete, uint nTmpFileNum)
{
   strcopy(szClExt, pszExt);
   bClAutoDel = !bNoAutoDelete;
   nClNum     = nTmpFileNum;
   pszClName  = 0;
}

SFTmpFile::~SFTmpFile()
{
   if (pszClName != 0) {
      if (bClAutoDel && fileExists(pszClName))
         remove(pszClName);
      delete [] pszClName;
      pszClName = 0;
   }
}

void SFTmpFile::setTmpDir(char *pszDir)
{
   pszTmpDir = pszDir;
}

bool SFTmpFile::tmpDirWasSet( )
{
   return pszTmpDir ? 1 : 0;
}

// uses szLineBuf
char *SFTmpFile::name()
{
   if (!pszClName) {
      char *psz = pszTmpDir;
      if (!psz) psz = getenv("TEMP");
      if (!psz) psz = getenv("TMP");
      #ifndef _WIN32
      if (!psz) psz = str("/tmp");
      #endif
      if (!psz)
         { perr("cannot create temporary file: no TEMP or TMP environment variable found.\n"); return 0; }
      pszClName = new char[MAX_LINE_LEN+10];
      if (joinPath(pszClName, MAX_LINE_LEN-20, psz, str("zz-tmp-sfk-")))
         { perr("cannot create temporary file.\n"); return 0; }
      int nlen = strlen(pszClName);
      uint uprocid = currentProcessID();
      if (nClNum > 0)
         sprintf(pszClName+nlen, "%03u%s", nClNum, szClExt);
      else
         sprintf(pszClName+nlen, "%03u%s", uprocid, szClExt);
      ncnt = (ncnt+1) % 3;
   }
   if (cperm.showtmp) {
       info.clear();
       setTextColor(nGlblTimeColor);
       oprintf("using temporary file: %s\n", pszClName);
       setTextColor(-1);
   }
   return pszClName;
}

// - - - sfk file selection

// only for processDirParms and walkAllTrees:
int nGlblError = 0; // flag from walkAllTrees
StringTable *pGlblFileParms = 0;
char **apGlblFileParms = 0;
int  nGlblFileParms    = 0;
bool bGlblHaveMixedDirFileList = 0;

enum eDirCommands {
   eCmd_CopyDir      = 1,
   eCmd_FreezeDir    = 2,
   eCmd_Undefined    = 0xFF
};

FileSet::FileSet()
 : clRootDirs("RootDirs"),
   clDirMasks("DirMasks"),
   clFileMasks("FileMasks")
{
   nClCurDir   =  0;
   nClCurLayer = -1;
   pClLineBuf  = new char[MAX_LINE_LEN+10];
   resetAddFlags();
}

FileSet::~FileSet() {
   shutdown();
}

void FileSet::resetAddFlags() {
   bClGotAllMask = 0;
   bClGotPosFile = 0;
   bClGotNegFile = 0;
}

void FileSet::reset()
{
   nClCurDir   =  0;
   nClCurLayer = -1;
   resetAddFlags();
   clRootDirs.reset();
   clDirMasks.reset();
   clFileMasks.reset();
}

void FileSet::shutdown() {
   reset();
   delete [] pClLineBuf;
   pClLineBuf = 0;
}

char* FileSet::firstFileMask() {
   Array &rMasks = fileMasks();
   if (!rMasks.isStringSet(0)) return str("*");
   return rMasks.getString(0);
}

int FileSet::getDirCommand() {
   return clRootDirs.getLong(1, nClCurDir, __LINE__);
}

/*
   A "layer" is a set of dir and file masks, which may be
   referenced by one or many root directories.
*/

void FileSet::dump()
{__
   printf("=== fileset begin ===\n");

   for (int i1=0; clRootDirs.isStringSet(0,i1); i1++)
   {
      int nLayer = clRootDirs.getLong(2,i1, __LINE__);
      int nCmd   = 0;
      if (clRootDirs.isLongSet(1,i1))
           nCmd   = clRootDirs.getLong(1,i1, __LINE__);
      printf("] ROOT \"%s\" -> layer %d, cmd %d\n",
         clRootDirs.getString(0,i1),
         nLayer,
         nCmd
         );
   }

   for (int iLayer=0; clDirMasks.hasRow(iLayer); iLayer++)
   {
      printf("] layer %d:\n", iLayer);

      printf("]  dmsk:\n");
      for (int i5=0; clDirMasks.isStringSet(iLayer, i5); i5++)
         printf("]   %s\n",clDirMasks.getString(iLayer, i5));

      printf("]  fmsk:\n]   ");
      for (int i6=0; clFileMasks.isStringSet(iLayer, i6); i6++)
         printf("%s, ",clFileMasks.getString(iLayer, i6));
      printf("\n");
   }

   printf("=== fileset end ===\n");
}

char *FileSet::currentInfoLine(int iLine)
{
   pClLineBuf[0] = '\0';
 
   // required:
   //  nClCurDir   is set
   //  nClCurLayer is set
   if (!hasRoot(nClCurDir))
      return str("");
   if (nClCurLayer < 0)
      return str("");

   int iLayer = nClCurLayer;
   char *pszUnit = 0;

   switch (iLine)
   {
      case 1: // current root dir
         return getCurrentRoot();
 
      case 2: // current layer's dir mask set
      {
         for (int i=0; clDirMasks.isStringSet(iLayer, i); i++) {
            pszUnit = clDirMasks.getString(iLayer, i);
            if (!strcmp(pszUnit, "*"))
               continue;
            if (strlen(pClLineBuf) > MAX_LINE_LEN-100)
               break;
            mystrcatf(pClLineBuf, MAX_LINE_LEN-100, "%s ", pszUnit);
         }
         break;
      }

      case 3: // current layer's file mask set
      {
         for (int i=0; clFileMasks.isStringSet(iLayer, i); i++) {
            pszUnit = clFileMasks.getString(iLayer, i);
            if (!strcmp(pszUnit, "*"))
               continue;
            if (strlen(pClLineBuf) > MAX_LINE_LEN-100)
               break;
            mystrcatf(pClLineBuf, MAX_LINE_LEN-100, "%s ", pszUnit);
         }
         break;
      }
   }
 
   return pClLineBuf;
}

// like dump, but easier representation, using pinf
void FileSet::info(void (*pout)(int nrectype, char *pline))
{
   char *psml = szLineBuf;
   char *pbig = (char*)abBuf;

   for (int i1=0; clRootDirs.isStringSet(0,i1); i1++)
   {
      int nLayer = clRootDirs.getLong(2,i1, __LINE__);
      int nCmd   = 0;
      if (clRootDirs.isLongSet(1,i1))
           nCmd   = clRootDirs.getLong(1,i1, __LINE__);

      sprintf(pbig, "directory tree %d:", i1+1);
      pout(1, pbig);

      pout(2, clRootDirs.getString(0,i1));

      // list masks used by that tree, possibly redundant
      int iLayer = nLayer;

      pbig[0] = '\0';
      int i5=0;
      for (; clDirMasks.isStringSet(iLayer, i5); i5++) {
         strcat(pbig, clDirMasks.getString(iLayer, i5));
         strcat(pbig, " ");
      }
      if (i5) {
         sprintf(psml, "... uses %d dir masks:", i5);
         pout(3, psml);
         pout(4, pbig);
      } else {
         pout(3, str("... uses no dir masks."));
      }

      pbig[0] = '\0';
      int i6=0;
      for (; clFileMasks.isStringSet(iLayer, i6); i6++) {
         strcat(pbig, clFileMasks.getString(iLayer, i6));
         strcat(pbig, " ");
      }
      if (i6) {
         sprintf(psml, "... uses %d file masks:", i6);
         pout(5, psml);
         pout(6, pbig);
      } else {
         pout(5, str("... uses no file masks."));
      }
   }
}

int FileSet::checkConsistency()
{__
   int nLayers = 0;
   for (int i0=0; clDirMasks.hasRow(i0); i0++)
      nLayers++;

   mtklog(("check consistency of %d layers", nLayers));

   int lRC = 0;

   int *pRefCnt = new int[nLayers+10];
   memset(pRefCnt, 0, (nLayers+10)*sizeof(int));
   // NO RETURN FROM HERE

   // count for each layer how often it's referenced
   for (int i1=0; clRootDirs.isStringSet(0,i1); i1++)
   {
      int iLayer = clRootDirs.getLong(2,i1, __LINE__);
      if (iLayer < 0 || iLayer >= nLayers) {
         perr("internal #60 %d\n", iLayer);
         lRC = 9;
         break;
      }
      mtklog(("root %d references layer %d", i1, iLayer));
      pRefCnt[iLayer]++;
   }

   // find unreferenced layers
   for (int i2=0; i2<nLayers; i2++) {
      if (!pRefCnt[i2]) {
         // if -any is specified, the first layer is used implicitely
         // to list important file extensions.
         if ((i2 == 0) && cs.anyused)
            continue;
         perr("wrong -dir and -file sequence (%d)\n", i2);
         pinf("specify -dir ... before -file\n");
         pinf("or type a single filename as first parameter.\n");
         lRC = 9;
      }
   }

   // NO RETURN UNTIL HERE
   delete [] pRefCnt;

   return lRC;
}

bool FileSet::anyRootAdded() {
   return (clRootDirs.numberOfEntries() > 0) ? 1 : 0;
}

// does the current root have any non-"*" file masks set?
bool FileSet::anyFileMasks()
{
   if (clFileMasks.numberOfEntries() <= 0)
      return false;

   // check if there is any non-"*" file mask
   for (int i=0; clFileMasks.isStringSet(i); i++)
   {
      char *pszMask = clFileMasks.getString(i);
      if (strcmp(pszMask, "*"))
         return true;
   }

   // if not, say no mask is set.
   return false;
}

bool FileSet::hasRoot(int iIndex) {
   bool bRC = clRootDirs.isStringSet(iIndex);
   // if (cs.debug) printf("] %d = hasRoot(%d)\n", bRC, iIndex);
   return bRC;
}

char* FileSet::setCurrentRoot(int iIndex)
{
   if (cs.debug) printf("] select root %d\n",iIndex);
   mtklog(("FileSet::setCurrentRoot(%d)", iIndex));
   nClCurDir = iIndex;
   // the root dir no. iIndex selects a specific layer.
   nClCurLayer = clRootDirs.getLong(2, iIndex, __LINE__);
   clRootDirs.setRow(0, __LINE__);
   // fully switch to current layer
   if (cs.debug) printf("]  select layer %d\n",nClCurLayer);
   mtklog(("   select layer %d",nClCurLayer));
   clDirMasks.setRow(nClCurLayer, __LINE__);
   clFileMasks.setRow(nClCurLayer, __LINE__);
   return getCurrentRoot();
}

// return current root or simply "" if none:
char *FileSet::root(bool braw) {
   if (!hasRoot(nClCurDir)) return braw ? 0 : (char*)"";
   return getCurrentRoot();
}

char* FileSet::getCurrentRoot()
{
   char *pszDirName = clRootDirs.getString(0, nClCurDir);
   // prefix by preroot, if any
   if (pszGlblPreRoot) {
      // printf("PRE %s ADD %s\n",pszGlblPreRoot,pszDirName);
      strcopy(szGlblMixRoot, pszGlblPreRoot);
      // avoid to append "./" as dir name
      if (!strncmp(pszDirName, glblDotSlash, 2))
         pszDirName += 2;
      // avoid to append "." as dir name
      if (!strcmp(pszDirName, "."))
         pszDirName++;
      // any local dir remaining to append?
      if (strlen(pszDirName)) {
         if (!endsWithPathChar(szGlblMixRoot) && !endsWithColon(szGlblMixRoot))
            strcat(szGlblMixRoot, glblPathStr);
         strcat(szGlblMixRoot, pszDirName);
      }
      pszDirName = szGlblMixRoot;
   }
   return pszDirName;
}

// just for webserv
int FileSet::changeSingleRoot(char *pszNew)
{
   if (clRootDirs.numberOfEntries(0) != 1)
      return 9+perr("cannot changeSingleRoot (%d/1)",clRootDirs.numberOfEntries(0));
   return clRootDirs.setString(0, 0, pszNew);
}

int FileSet::numberOfRootDirs() {
   return clRootDirs.numberOfEntries(0);
}

int FileSet::ensureBase(int nTraceLine) {
   mtklog(("fs: ensureBase %d", nTraceLine));
   // clRootDirs must have 3 rows
   while (!clRootDirs.hasRow(2)) {
      mtklog(("fs: ... add another row"));
      if (clRootDirs.addRow(nTraceLine))
         return 9;
   }
   return 0;
}

// a layer is one set of dir masks and file masks.
// - dir masks is a row in clDirMasks.
// - file masks is a row in clFileMasks.
// - command is 1:1 per directory root,
//   therefore cmd is stored in clRootDirs.
// - clRootDirs row 0 is the directory name.
// - clRootDirs row 1 is the directory command (zip, copy).
// - a directory tree references a layer.

int FileSet::beginLayer(bool bWithEmptyCommand, int nTraceLine)
{
   mtklog(("fs: beginlayer %d", nTraceLine));

   // reset state of add sequence controlling
   resetAddFlags();

   // ensure clRootDirs has two columns
   if (ensureBase(__LINE__)) return 9;
   // make space for new layer
   clDirMasks.addRow(__LINE__);
   clFileMasks.addRow(__LINE__);
   // count new layer
   nClCurLayer++;
   // printf("] BEGINLAYER: new current=%d [\n",nClCurLayer);

   // a "command" is "-copy" or "-zip", used only with freezeto.
   // all other commands will just fill the command column with dummys.
   if (bWithEmptyCommand) {
      // set dummy command "0" in row 1 of clRootDirs
      clRootDirs.addLong(1, 0, __LINE__);
   }
   return 0;
}

int FileSet::addRootDir(char *pszRoot, int nTraceLine,
   bool bNoCmdFillup, bool bAutoUseArc)
{
   mtklog(("fs: addRootDir %s from %d", pszRoot, nTraceLine));

   if (cs.debug) printf("] add root dir: %s, referencing layer: %d [tline %d]\n", pszRoot, nClCurLayer, nTraceLine);

   // prefix root: if -root is given.
   // purify root: no patch char appended.
   if (cs.rootdir[0]) {
      if (strcmp(pszRoot, ".")) // avoid mydir\.\first.txt
         joinPath(pClLineBuf, MAX_LINE_LEN, cs.rootdir, pszRoot);
      else
         strncpy(pClLineBuf, cs.rootdir, MAX_LINE_LEN);
      pClLineBuf[MAX_LINE_LEN] = '\0';
      if (cs.debug) printf("]  use prefixed: %s\n", pClLineBuf);
   } else {
      strncpy(pClLineBuf, pszRoot, MAX_LINE_LEN);
      pClLineBuf[MAX_LINE_LEN] = '\0';
   }

   pszRoot = pClLineBuf;

   if (endsWithPathChar(pszRoot)) {
      #ifdef _WIN32
      if (strlen(pszRoot) == 3 && pszRoot[1] == ':')
         { } // do not strip / from c:/
      else
      #endif
      if (strlen(pszRoot) > 1)
         pszRoot[strlen(pszRoot)-1] = '\0';
   }

   if (ensureBase(__LINE__)) return 9;
   if (nClCurLayer == -1) {
      if (cs.debug) printf("] impl. create first fileset layer\n");
      beginLayer(true, __LINE__);
   }
   // complete the current column
   int nMax = clRootDirs.numberOfEntries(0);
   if (!bNoCmdFillup)
      if (clRootDirs.numberOfEntries(1) < nMax)
          clRootDirs.addLong(1, 0, __LINE__); // add empty command
   if (clRootDirs.numberOfEntries(2) < nMax)
      return 9+perr("internal #20\n");

   // add another root dir, forward-referencing
   // the current layer in which masks will follow.
   clRootDirs.addString(0,pszRoot);
   clRootDirs.addLong(2,nClCurLayer, __LINE__);

   #ifdef VFILEBASE
   // implicite activation of zip travel by root dir:
   if (bAutoUseArc && endsWithArcExt(pszRoot, 2)) {
      cs.xelike     = 1;
      cs.travelzips = 1;
   }
   #endif // VFILEBASE

   return 0;
}  // FileSet::addRootDir

int FileSet::addDirCommand(int lCmd)
{
   if (ensureBase(__LINE__)) return 9;
   // one single -cmd may map to several root dirs.
   // example: -dir a1 a2 a3 -copy
   // therefore fill up until root dir names number reached.
   int nMax = clRootDirs.numberOfEntries(0);
   while (clRootDirs.numberOfEntries(1) < nMax)
      clRootDirs.addLong(1, lCmd, __LINE__);
   return 0;
}

int FileSet::addDirMask(char *pszMask)
{
   if (ensureBase(__LINE__)) return 9;
 
   // per definitionem,  ".ext" means ".ext/"
   // per definitionem, "!.ext" means "!.ext/"
   // per definitionem, "*.ext" means "*.ext/"
 
   int iMaskLen = strlen(pszMask);
   if (pszMask[0] == '.' && pszMask[iMaskLen-1] != glblPathChar)
   {
      snprintf(pClLineBuf, MAX_LINE_LEN-10, "%s%c", pszMask, glblPathChar);
      pszMask = pClLineBuf;
   }
   else
   if (   isNotChar(pszMask[0]) != 0
       && pszMask[1] == '.'
       && pszMask[iMaskLen-1] != glblPathChar)
   {
      snprintf(pClLineBuf, MAX_LINE_LEN-10, "%s%c", pszMask, glblPathChar);
      pszMask = pClLineBuf;
   }
   else
   if (   pszMask[0] == glblWildChar
       && pszMask[1] == '.'
       && pszMask[iMaskLen-1] != glblPathChar)
   {
      snprintf(pClLineBuf, MAX_LINE_LEN-10, "%s%c", pszMask, glblPathChar);
      pszMask = pClLineBuf;
   }
 
   clDirMasks.addString(pszMask);
 
   return 0;
}

int FileSet::addFileMask(char *pszMask)
{
   // if (cs.debug) printf("] addFileMask %s\n", pszMask);
   if (ensureBase(__LINE__)) return 9;

   // per definitionem,  ".ext" means ".ext/"
   // per definitionem, "!.ext" means "!.ext/"
 
   int iMaskLen = strlen(pszMask);
   if (pszMask[0] == '.' && pszMask[iMaskLen-1] != glblPathChar)
   {
      snprintf(pClLineBuf, MAX_LINE_LEN-10, "%s%c", pszMask, glblPathChar);
      pszMask = pClLineBuf;
   }
   else
   if (   isNotChar(pszMask[0]) != 0
       && pszMask[1] == '.'
       && pszMask[iMaskLen-1] != glblPathChar)
   {
      snprintf(pClLineBuf, MAX_LINE_LEN-10, "%s%c", pszMask, glblPathChar);
      pszMask = pClLineBuf;
   }

   #ifdef VFILEBASE
   if (!cs.shallowzips && cs.xelike && cs.travelzips && maskEndsWithArcExt(pszMask, 3))
   {
      pinf("file mask \"%s\" may produce no results, because\n",pszMask);
      char *pext = strrchr(pszMask, '.');
      if (!pext) pext = pszMask;
      pinf("option -arc is set, treating %s files as directories.\n",pext);
      pinf("say -dir ... %c%s -file ... instead to use %s as a path mask.\n",glblWildChar,pext,pszMask);
   }
   #endif // VFILEBASE

   if (!strcmp(pszMask, "-all") || isWildStr(pszMask)) {
      if (bClGotAllMask)
         return 9+perr("-all or %c supplied multiple times.\n", glblWildChar);
      if (bClGotPosFile) {
         if (bGlblAllowAllPlusPosFile)
            pwarn("wrong sequence: positive file pattern already given, specify \"%s\" before this.\n", pszMask);
         else
            pwarn("positive file pattern already given, \"%s\" is unexpected.\n", pszMask);
      }
      if (bClGotNegFile)
         return 9+perr("wrong sequence: negative file pattern already given, specify \"%s\" before this.\n", pszMask);
      // map to * internally:
      pszMask = str("*");    // no wildstr, using real * internally
      bClGotAllMask = 1;
   }
   else
   if (isNotChar(pszMask[0])) {
      // negative file pattern: if it is the very first pattern
      if (!bClGotAllMask && !bClGotPosFile && !bClGotNegFile)
         clFileMasks.addString(str("*")); // IMPLICITELY. no wildstr.
      bClGotNegFile = 1;
   }
   else {
      // positive file pattern:
      if (bClGotAllMask && !bGlblAllowAllPlusPosFile)
         pwarn("-all or * already given, \"%s\" has no effect.\n", pszMask);
      if (bClGotNegFile)
         return 9+perr("wrong sequence: negative file pattern already given, specify \"%s\" before this.\n", pszMask);
      bClGotPosFile = 1;
   }

   clFileMasks.addString(pszMask);
   return 0;
}

int FileSet::autoCompleteFileMasks(int nWhat)
{
   if (cs.debug) printf("] autocomplete %d:\n", nWhat);

   if (nWhat & 1)
   for (int irow=0; clFileMasks.hasRow(irow); irow++) {
      if (clFileMasks.setRow(irow, __LINE__)) return 9;
      if (clFileMasks.numberOfEntries() == 0) {
         if (cs.debug) printf("]  yes, at layer %d\n",irow);
         if (clFileMasks.addString(str("*"))) return 9;   // no wildstr
      }
   }

   if (nWhat & 2)
   if (clRootDirs.numberOfEntries() == 0) {
      if (cs.debug) printf("] adding dir .\n");
      addRootDir(str("."), __LINE__, false);
   }

   return 0;
}

void FileSet::setBaseLayer()
{
   clRootDirs.setRow(0, __LINE__);
   clDirMasks.setRow(0, __LINE__);
   clFileMasks.setRow(0, __LINE__);
}

FileList::FileList()  { }
FileList::~FileList() { }

void FileList::reset()
{
   clNames.resetEntries();
   clRoots.resetEntries();
   clTimes.resetEntries();
   clSizes.resetEntries();
}

int FileList::addFile(char *pszAbsName, char *pszRoot, num nTimeStamp, num nSize, char cSortedBy)
{__ _p("sf.addfile")

   // IF filename starts with ".\\", skip this part
   if (!strncmp(pszAbsName, glblDotSlash, strlen(glblDotSlash)))
      pszAbsName += strlen(glblDotSlash);

   // insert sorted by (1)name, (2)time, (3)size?
   int nInsPos = -1;

   if (cSortedBy == 'T' || cSortedBy == 't') {
      // find insert by time position
      int nCnt = clTimes.numberOfEntries();
      int i=0;
      for (i=0; i<nCnt; i++)
         if (cSortedBy=='T' && (clTimes.getEntry(i, __LINE__) > nTimeStamp))
            break;
         else
         if (cSortedBy=='t' && (clTimes.getEntry(i, __LINE__) < nTimeStamp))
            break;
      if (i < nCnt)
         nInsPos = i;
   }

   if (cSortedBy == 'S' || cSortedBy == 's') {
      // find insert by size position
      int nCnt = clSizes.numberOfEntries();
      int i=0;
      for (i=0; i<nCnt; i++)
         if (cSortedBy=='S' && (clSizes.getEntry(i, __LINE__) > nSize))
            break;
         else
         if (cSortedBy=='s' && (clSizes.getEntry(i, __LINE__) < nSize))
            break;
      if (i < nCnt)
         nInsPos = i;
   }

   if (cSortedBy == 'N' || cSortedBy == 'n') {
      // find insert by name position
      int nCnt = clNames.numberOfEntries();
      int i=0;
      for (i=0; i<nCnt; i++) {
         char *psz1 = clNames.getEntry(i, __LINE__);
         char *psz2 = pszAbsName;
         // both psz1 and psz2 point to a mixed string with a prefix.
         // need to find the actual filename first:
         int npre1 = atol(psz1)+6; if (npre1 < (int)strlen(psz1)) psz1 += npre1;
         int npre2 = atol(psz2)+6; if (npre2 < (int)strlen(psz2)) psz2 += npre2;
         int ncmp  = cs.usecase ? strcmp(psz1,psz2) : mystricmp(psz1, psz2);
         if (cSortedBy=='N' && ncmp > 0)
            break;
         else
         if (cSortedBy=='n' && ncmp < 0)
            break;
      }
      if (i < nCnt)
         nInsPos = i;
   }

   if (clNames.addEntry(pszAbsName, nInsPos)) return 9;
   if (clRoots.addEntry(pszRoot   , nInsPos)) return 9;
   if (clTimes.addEntry(nTimeStamp, nInsPos)) return 9;
   if (clSizes.addEntry(nSize     , nInsPos)) return 9;

   return 0;
}

// checkAndMark checks the filename and size,
// but NOT the rather complicated timestamps.
int FileList::checkAndMark(char *pszName, num nSize) {
   int nEntries = clNames.numberOfEntries();
   for (int i=0; i<nEntries; i++) {
      char *psz = clNames.getEntry(i, __LINE__);
      if ((psz[0] != 0) && !strcmp(psz, pszName)) {
         num nSize2 = clSizes.getEntry(i, __LINE__);
         if (nSize != nSize2)
            return 2; // on size mismatch, do NOT mark as done.
         psz[0] = '\0';
         return 0;
      }
   }
   return 1;
}

// - - - command chain and help system

// sfk cmd support
#define MAX_HELPCMD_TEXT 1000
char szGlblHelpCmdPat[50]; // e.g. [1234]
char szGlblHelpCmdText[MAX_HELPCMD_TEXT+100];
int  iGlblHelpCmd = 0;
int  iGlblHelpCmdCollectState = 0;
int  iGlblHelpCmdIndent = 0;

void addHelpCmdLine(char *pszRawIn, char *pszForm)
{
   /*
      raw
         "      #sfk xex in.csv "_[lstart]*\t*\t*_<row>\n <artist>[part2]{LF}"
         ...
         "         then reformat this to xml data. [123] \n"

      form
         sfk xex in.csv "_[lstart]*\t*\t*_<row>\n <artist>[part2]
         ...
         then reformat this to xml data. [123]{CR/LF}
   */

   // is it the searched reference?
   char *psz = pszForm + strlen(pszForm);
   while (psz > pszForm && (psz[-1]=='\r' || psz[-1]=='\n'))
      psz--;
   if (psz > pszForm && psz[-1] == ']' && strstr(pszForm, szGlblHelpCmdPat))
   {
      // yes, stop further collection
      iGlblCollectCmd = 2;
      return;
   }

   // collect example text
   bool bexamp=0;
   char *pszRaw = pszRawIn;
   while (*pszRaw == ' ')
      pszRaw++;
   if (*pszRaw == '#') {
      int iCurIndent = (int)(pszRaw - pszRawIn);
      if (!strncmp(pszRaw, "#sfk", 4)) {
         iGlblHelpCmdIndent = iCurIndent;
         bexamp = 1;
      } else {
         if (iCurIndent==iGlblHelpCmdIndent || iCurIndent==iGlblHelpCmdIndent+1)
            bexamp = 1;
      }
   }

   switch (iGlblHelpCmdCollectState)
   {
      case 0:  // seeking for next example start
         if (bexamp) {
            iGlblHelpCmdCollectState=1;
            break;
         }
         // any text
         iGlblHelpCmdCollectState=2;
         return;
 
      case 1:  // within example text lines
         if (bexamp)
            break;
         // any text
         iGlblHelpCmdCollectState=2;
         return;

      case 2:  // seeking for reference, or next example start
         if (bexamp) {
            szGlblHelpCmdText[0] = '\0';
            iGlblHelpCmdCollectState=1;
            break;
         }
         // any text
         return;
   }

   // append another example line

   int iquotes = 0;
   for (char *psz2 = szGlblHelpCmdText; *psz2; psz2++)
      if (*psz2 == '\"')
         iquotes++;
   if (szGlblHelpCmdText[0] != 0 && (iquotes & 1) == 0)
      strcat(szGlblHelpCmdText, " ");

   char *pSrcCur = pszForm;
   char *pSrcMax = pszForm + strlen(pszForm);
   while (*pSrcCur == ' ')
      pSrcCur++;
   while (pSrcMax > pszForm && (pSrcMax[-1]=='\r' || pSrcMax[-1]=='\n'))
      pSrcMax--;

   int iDstLen = strlen(szGlblHelpCmdText);
   int iDstRem = MAX_HELPCMD_TEXT - iDstLen;
   int iSrcLen = pSrcMax - pSrcCur;
   if (iSrcLen < 1 || iSrcLen+10 > iDstRem) {
      printf("example overflow: %s\n", pszForm);
      return;
   }

   memcpy(szGlblHelpCmdText+iDstLen, pSrcCur, iSrcLen);
   szGlblHelpCmdText[iDstLen+iSrcLen] = '\0';
}

void addHelpCmdLines(char *pszRaw, char *pszForm)
{
   char szBuf1[1100],szBuf2[1100];

   while (*pszRaw && *pszForm)
   {
      char *pszRaw2 = strchr(pszRaw, '\n');
      if (!pszRaw2)
            pszRaw2 = pszRaw + strlen(pszRaw);
      int iLenRaw = pszRaw2 - pszRaw;
      if (iLenRaw > 1000) {
         printf("help.overflow.1 %s\n", pszRaw);
         return;
      }

      char *pszForm2 = strchr(pszForm, '\n');
      if (!pszForm2)
            pszForm2 = pszForm + strlen(pszForm);
      int iLenForm = pszForm2 - pszForm;
      if (iLenForm > 1000) {
         printf("help.overflow.2 %s\n", pszForm);
         return;
      }

      memcpy(szBuf1, pszRaw, iLenRaw);
      szBuf1[iLenRaw] = '\0';

      memcpy(szBuf2, pszForm, iLenForm);
      szBuf2[iLenForm] = '\0';

      addHelpCmdLine(szBuf1, szBuf2);

      if (iGlblCollectCmd == 2)
         break;

      pszRaw = pszRaw2;
      if (*pszRaw)
         pszRaw++;

      pszForm = pszForm2;
      if (*pszForm)
         pszForm++;
   }
}

int chainAddLine(char *pszText, char *pszAttr, bool bSplitByLF)
{
   // mtklog(("chain-add: \"%.50s\"", pszText));
   return chain.addLine(pszText, pszAttr, bSplitByLF);
}

int CommandChaining::hasFile(char *psz) {
   return outfiles->hasEntry(psz);
}

int CommandChaining::addFile(Coi &ocoi) {
   return outfiles->addEntry(ocoi);
}

Coi *CommandChaining::getFile(int nIndex) {
   return infiles->getEntry(nIndex, __LINE__);
}

void CommandChaining::dumpContents()
{__
   if (usedata) {
      printf("[indata]\n");
      for (int i=0; i<chain.indata->numberOfEntries(); i++) {
         char *pattr = str("");
         char *ptext = chain.indata->getEntry(i, __LINE__, &pattr);
         if (ptext)
            printColorText(ptext, pattr, 1); // with lf
      }
   }

   if (usefiles) {
      printf("[infiles]\n");
      for (int i=0; i<chain.numberOfInFiles(); i++) {
         Coi *pcoi = chain.getFile(i);
         if (pcoi) {
            oprintf("%s\n", pcoi->name());
         }
      }
   }
}

// uses print buffers from printx
int CommandChaining::print(char cattrib, int nflags, cchar *pszFormat, ...)
{__
   va_list argList;
   va_start(argList, pszFormat);

   #ifdef SFKPRINTREDIR
   bool bcollect = colany();
   #else
   bool bcollect = coldata;
   #endif

   bool bWithPostLF = (nflags & 1) ? 1 : 0;
   bool bWithPreLF  = (nflags & 2) ? 1 : 0;
   bool bMultiLine  = (nflags & 4) ? 1 : 0;

   // prepare text
   ::vsnprintf(szClPreBuf, sizeof(szClPreBuf)-10, pszFormat, argList);
   szClPreBuf[sizeof(szClPreBuf)-10] = '\0';

   if (bMultiLine)
   {
      // full flexible line split
      if (!bcollect && !cs.outfile)
         info.clear();
 
      char *psz1 = szClPreBuf;
      while (psz1 && *psz1)
      {
         char *psz2 = strchr(psz1, '\n');
         if (psz2) {
            // intermediate record
            int n = psz2-psz1;
            if (n > MAX_LINE_LEN) n = MAX_LINE_LEN;
            memcpy(szPrintBuf2, psz1, n);
            szPrintBuf2[n] = '\0';
            trimCR(szPrintBuf2);
            psz2++;
         } else {
            // last record
            strcopy(szPrintBuf2, psz1);
         }
 
         int nlen = strlen(szPrintBuf2);
         memset(szClPreAttr, cattrib, nlen);
         szClPreAttr[nlen] = '\0';
 
         if (bcollect)
            addLine(szPrintBuf2, szClPreAttr);
         else
         if (cs.outfile)
            fprintf(cs.outfile, "%s\n", szPrintBuf2);
         else
            printColorText(szPrintBuf2, szClPreAttr, 1);
 
         psz1 = psz2;
      }
   }
   else
   {
      // single line print

      // prepare mono color
      int nlen = strlen(szClPreBuf);
      memset(szClPreAttr, cattrib, nlen);
      szClPreAttr[nlen] = '\0';
 
      if (bcollect) {
         if (bWithPreLF)
            addLine(str(""), str(""));
         if (bWithPostLF) {
            return addLine(szClPreBuf, szClPreAttr);
         } else {
            return addToCurLine(szClPreBuf, szClPreAttr, bWithPreLF);
         }
      } else {
         info.clear();
         if (bWithPreLF)
            printf("\n");
         printColorText(szClPreBuf, szClPreAttr, bWithPostLF);
      }
   }

   return 0;
}

// uses print buffers from printx
int CommandChaining::printFile(cchar *pszOutFile, bool bWriteFile, cchar *pszFormat, ...)
{__
   int nrc = 0;

   #ifdef SFKPRINTREDIR
   bool bcollect = colany();
   #else
   bool bcollect = coldata;
   #endif

   // there should be no redirect file open
   if (cs.outfile) return 9+perr("redirect file already open");

   if (bWriteFile) {
      // isolate path, create all directories
      strcopy(szPrintBuf2, pszOutFile);
      char *psz1 = strrchr(szPrintBuf2, glblPathChar);
      if (psz1) {
         *psz1 = '\0';
         if ((nrc = createSubDirTree(szPrintBuf2, str(""), 0)))
            return nrc+perr("cannot create dir: %s\n", szPrintBuf2);
      }
 
      // init redirect outfile
      if (!(cs.outfile = fopen(pszOutFile, "w")))
         return 9+perr("cannot write: %s\n", pszOutFile);
   } else {
      // just simulate the file writing:
      printf("\n:file:\n%s\n", pszOutFile);
   }

   // NO RETURN WITHOUT CONDITIONAL FILE CLOSE BEGIN

   va_list argList;
   va_start(argList, pszFormat);

   // prepare text
   ::vsnprintf(szClPreBuf, sizeof(szClPreBuf)-10, pszFormat, argList);
   szClPreBuf[sizeof(szClPreBuf)-10] = '\0';

   if (!bcollect && !cs.outfile)
      info.clear();

   char *psz1 = szClPreBuf;
   while (psz1 && *psz1)
   {
      char *psz2 = strchr(psz1, '\n');
      if (psz2) {
         // intermediate record
         int n = psz2-psz1;
         if (n > MAX_LINE_LEN) n = MAX_LINE_LEN;
         memcpy(szPrintBuf2, psz1, n);
         szPrintBuf2[n] = '\0';
         psz2++;
      } else {
         // last record
         strcopy(szPrintBuf2, psz1);
      }

      int nlen = strlen(szPrintBuf2);
      memset(szClPreAttr, ' ', nlen);
      szClPreAttr[nlen] = '\0';

      if (bcollect)
         addLine(szPrintBuf2, szClPreAttr);
      else
      if (cs.outfile)
         fprintf(cs.outfile, "%s\n", szPrintBuf2);
      else
         printColorText(szPrintBuf2, szClPreAttr, 1);

      psz1 = psz2;
   }

   // NO RETURN WITHOUT FILE CLOSE END

   if (bWriteFile && cs.outfile) {
      fclose(cs.outfile);
      cs.outfile = 0;
      printf("written: %s\n", pszOutFile);
   }

   return nrc;
}

// raw printf like method. no colors, no special options,
// but it splits multiline text into many records.
// one-line text SHOULD be terminated with LF.
int CommandChaining::print(cchar *pszFormat, ...)
{__
   va_list argList;
   va_start(argList, pszFormat);

   #ifdef SFKPRINTREDIR
   bool bcollect = colany();
   #else
   bool bcollect = coldata;
   #endif

   // prepare text
   ::vsnprintf(szClPreBuf, sizeof(szClPreBuf)-10, pszFormat, argList);
   szClPreBuf[sizeof(szClPreBuf)-10] = '\0';

   if (!bcollect && !cs.outfile)
      info.clear();

   char *psz1 = szClPreBuf;
   while (psz1 && *psz1)
   {
      char *psz2 = strchr(psz1, '\n');
      if (psz2) {
         // intermediate record
         int n = psz2-psz1;
         if (n > MAX_LINE_LEN) n = MAX_LINE_LEN;
         memcpy(szPrintBuf2, psz1, n);
         szPrintBuf2[n] = '\0';
         psz2++;
      } else {
         // last record
         strcopy(szPrintBuf2, psz1);
      }

      int nlen = strlen(szPrintBuf2);
      memset(szClPreAttr, ' ', nlen);
      szClPreAttr[nlen] = '\0';

      if (bcollect)
         addLine(szPrintBuf2, szClPreAttr);
      else
      if (cs.outfile)
         fprintf(cs.outfile, "%s\n", szPrintBuf2);
      else
         printColorText(szPrintBuf2, szClPreAttr, 1);

      psz1 = psz2;
   }

   return 0;
}

int CommandChaining::openOverallOutputFile(cchar *pszMode)
{
   if (!cs.tomask)
      return 0;
   if (!cs.tomaskfile)
      return 9+perr("use -tofile but not -to.");
   if (chain.colany())
      return 9+perr("-tofile excludes any further chain processing.");
   if (!(cs.outfile = fopen(cs.tomask, pszMode)))
      return 9+perr("cannot open for writing: %s", cs.tomask);
   cs.overallOutFilename = cs.tomask;
   cs.tomask = 0; // avoid per infile handling by execSingleFile
   return 0;
}

void CommandChaining::closeOverallOutputFile(int iDoneFiles)
{
   if (cs.outfile == 0)
      return;
   fclose(cs.outfile);
   cs.outfile = 0;

   if (!cs.quiet && cs.overallOutFilename) {
      num nbytes = getFileSize(cs.overallOutFilename);
      cchar *punit = "bytes";
      num   ndiv   = 1;
      if (nbytes > 1000000)
         {  ndiv   = 1000000; punit = "mb"; }
      else
      if (nbytes > 1000)
         {  ndiv   = 1000; punit = "kb"; }
      if (iDoneFiles)
         info.print("wrote %d files to %s (%s %s).\n",
            iDoneFiles, cs.overallOutFilename,
            numtoa(nbytes/ndiv),punit);
      else
         info.print("wrote %s with %s %s.\n",
            cs.overallOutFilename,
            numtoa(nbytes/ndiv),punit);
   }
}

int CommandChaining::convInDataToInFiles()
{__
   // user wants to convert text to filename list
   for (int i=0; i<indata->numberOfEntries(); i++)
   {
      char *psz1 = indata->getEntry(i, __LINE__);
      // strip LFs (if any) from indata. we will drop
      // all indata records below, therefore edit directly:
      int nlen = strlen(psz1);
      if (nlen > 0 && psz1[nlen-1] == '\n')
          psz1[nlen-1] = '\0';
      // sfk1934: guess filename root on text data
      char *pszroot = getAbsPathStart(psz1);
      // printf("root %s for %s\n",pszroot,psz1);
      Coi ocoi(psz1, pszroot); // sfk1934 fromclip +copy support
      infiles->addEntry(ocoi); // is copied
   }
   indata->resetEntries();
   if (cs.tracechain)
      printf("[chain converts text data to filename list]\n");
   usefiles = 1;
   usedata  = 0;
   return 0;
}

int CommandChaining::addBinary(uchar *pData, int iSize)
{
   int iRemain   = iSize;
   int iRecChars = (sizeof(szClBinBuf)-100);
   int iRecBytes = iRecChars/2;
 
   const char *ptohex = "0123456789ABCDEF";

   int iSrcOff = 0;
   int iDstOff = iClBinBufUsed;

   while (iRemain > 0)
   {
      int iPart = mymin(iRemain, iRecBytes);

      for (int iByte=0; iByte<iPart; iByte++)
      {
         if (iDstOff+2 >= iRecChars)
         {
            szClBinBuf[iDstOff] = '\0';
            iDstOff = iClBinBufUsed = 0;
            if (outdata->addEntry(szClBinBuf, str("")))
               return 9;
         }

         uchar u = pData[iSrcOff+iByte];
         szClBinBuf[iDstOff+0] = ptohex[(u>>4)&0xF];
         szClBinBuf[iDstOff+1] = ptohex[(u   )&0xF];
         nClOutCheckSum = ((nClOutCheckSum << 1) ^ (nClOutCheckSum >> 31)) ^ u;

         iDstOff += 2;
      }

      iSrcOff += iPart;
      iRemain -= iPart;
      nClOutBinarySize += iPart;
   }

   iClBinBufUsed = iDstOff;
 
   return 0;
}

// guarantees zero termination with 100 bytes tolerance
uchar *CommandChaining::loadBinary(num &rSize)
{
   if (nClInBinarySize < 1)
      return 0;

   uchar *pData = new uchar[nClInBinarySize+100];
   if (!pData)
      { perr("out of memory while collecting binary chain data (%s)", numtoa(nClInBinarySize)); return 0; }
   memset(pData, 0, nClInBinarySize+100);

   uchar *pDstCur = pData;
   uchar *pDstMax = pData + nClInBinarySize;

   bool bError = 0;

   num nBytes = 0;
 
   uint nTmpSum = 0;

   for (int i=0; i<chain.indata->numberOfEntries(); i++)
   {
      char *pattr = str("");
      char *ptext = chain.indata->getEntry(i, __LINE__, &pattr);
      if (!ptext)
         break;

      while (ptext[0] && ptext[1])
      {
         uchar u = twoHexToUChar(ptext, bError);
         if (bError) {
            delete [] pData;
            perr("invalid chain binary data format: %s", ptext);
            pinf("a previous command may have sent two conflicting data.\n");
            return 0;
         }
         if (pDstCur >= pDstMax) {
            // should not occur
            delete [] pData;
            perr("output overflow while collecting chain binary data");
            return 0;
         }
         nTmpSum = ((nTmpSum << 1) ^ (nTmpSum >> 31)) ^ u;
         *pDstCur++ = u;
         nBytes++;
         ptext += 2;
      }
   }

   if (nBytes != nClInBinarySize) {
      delete [] pData;
      perr("data size mismatch while collecting chain binary data (%d/%d)",(int)nBytes,(int)nClInBinarySize);
      return 0;
   }
   if (nTmpSum != nClInCheckSum) {
      delete [] pData;
      perr("checksum mismatch while collecting chain binary data (%X/%X)",nTmpSum,nClInCheckSum);
      return 0;
   }

   rSize = nBytes;

   return pData;
}

int CommandChaining::addBlockAsLines(char *pData, int iData)
{
   int isubrc=0;
   if ((isubrc = addStreamAsLines(1,0,0)))
      return isubrc;
   if ((isubrc = addStreamAsLines(2,pData,iData)))
      return isubrc;
   if ((isubrc = addStreamAsLines(3,0,0)))
      return isubrc;
   return 0;
}

int CommandChaining::addStreamAsLines(int iCmd, char *pData, int iData)
{
   static char aCache[MAX_LINE_LEN+100];
   static int  iCache=0;

   if (iCmd==1) { // init
      iCache=0;
      return 0;
   }

   if (iCmd==2) { // add
      char *pSrcCur=pData;
      char *pSrcMax=pData+iData;
      while (pSrcCur<pSrcMax) {
         char c=*pSrcCur++;
         if (c=='\n') {
            // regular line end
            aCache[iCache]='\0';
            removeCRLF(aCache); // fix sfk1912
            addLine(aCache,str(""),0);
            iCache=0;
            continue;
         }
         aCache[iCache++]=c;
         if (iCache >= MAX_LINE_LEN) {
            // hard wrap
            aCache[iCache]='\0';
            addLine(aCache,str(""),0);
            iCache=0;
            continue;
         }
      }
   }

   if (iCmd==3) { // flush, close
      if (iCache>0) {
         aCache[iCache]='\0';
         addLine(aCache,str(""),0);
         iCache=0;
      }
   }

   return 0;
}

int CommandChaining::addLine(char *pszText, char *pszAttr, int iSplitByLF)
{
   mtklog(("addl.text: %d %04d \"%s\"", iSplitByLF, strlen(pszText), pszText));
   mtklog(("addl.attr: %d %04d \"%s\"", iSplitByLF, strlen(pszAttr), pszAttr));

   if (pszText[0] && iSplitByLF)
   {
      // detect if the line contains multiple linefeeds.
      // if so, create multiple records.
      bool btermlf = 0;
      char *psz  = pszText;
      int nalen = strlen(pszAttr);
      while (*psz)
      {
         char *psz2 = psz;
         while (*psz2 && *psz2 != '\n')
            psz2++;
         int nlen = psz2-psz;
         if (nlen > MAX_LINE_LEN) nlen = MAX_LINE_LEN;

         // isolate text part
         memcpy(szClBuf, psz, nlen);
         szClBuf[nlen] = '\0';
         removeCRLF(szClBuf); // just in case

         // isolate attribute part
         int noff = psz-pszText;
         if (noff < nalen) {
            strncpy(szClAttr, pszAttr+noff, nlen);
            szClAttr[nlen] = '\0';
         } else {
            szClAttr[0] = '\0';
         }

         // then store both parts
         outdata->addEntry(szClBuf, szClAttr);

         if (*psz2) {
            psz2++; // skip lf
            if (!*psz2) btermlf = 1;
         }
         psz = psz2;
      }
      if (btermlf && (iSplitByLF > 1)) {
         // the text was ended by a LF. this should produce
         // an empty line only if SplitByLF == 2.
         outdata->addEntry(str(""), str(""));
      }
   } else {
      // used also for empty strings, to add empty line
      outdata->addEntry(pszText, pszAttr);
   }
   return 0;
}

int CommandChaining::addToCurLine(char *pszWords, char *pszAttr, bool bNewLine)
{
   mtklog(("addc.text: %d %04d \"%s\"", bNewLine, strlen(pszWords), pszWords));
   mtklog(("addc.attr: %d %04d \"%s\"", bNewLine, strlen(pszAttr ), pszAttr ));

   if (!outdata->numberOfEntries() || bNewLine)
       if (outdata->addEntry(str(""), str("")))
         return 9;

   // append actual text
   {
      int nidx = outdata->numberOfEntries()-1;
      if (nidx < 0) return 9+perr("internal 200706102039");

      char *pAttr  = 0;
      char *pszCur = outdata->getEntry(nidx, __LINE__, &pAttr);
      strcopy(szClBuf, pszCur);
      if (pAttr) strcopy(szClAttr, pAttr);
      else szClAttr[0] = '\0';

      int icurlen = strlen(szClBuf);
      int nremain = MAX_LINE_LEN - icurlen - 1;

      // append text
      int naddlen = strlen(pszWords);
      if (naddlen > nremain) {
         if (!btold1) {
            btold1 = 1;
            perr("line buffer overflow: cannot join \"%s\" and following\n", pszWords);
            pinf("buffered lines may have %d characters max.\n", MAX_LINE_LEN-10);
         }
         return 1;
      }

      // and attributes as well, length limited by text
      int nattlen = strlen(pszAttr);
      if (nattlen > naddlen) nattlen = naddlen;

      mystrncpy(szClBuf+icurlen, pszWords, naddlen); // sfk200 [2410131]
      szClBuf[icurlen+naddlen] = '\0';

      mystrncpy(szClAttr+icurlen, pszAttr, nattlen); // sfk200 [2410131]
      szClAttr[icurlen+nattlen] = '\0';

      outdata->setEntry(nidx, szClBuf, szClAttr);
   }

   return 0;
}

int CommandChaining::moveOutToIn(char *pszCmd)
{__

   // flush final part of binary add
   if (chain.coldata && iClBinBufUsed)
   {
      if (cs.tracechain)
         printf("[chain flushes %d binary data]\n", iClBinBufUsed/2);
      szClBinBuf[iClBinBufUsed] = '\0';
      iClBinBufUsed = 0;
      if (outdata->addEntry(szClBinBuf, str("")))
         return 9;
   }

   CoiTable *p1 = infiles;
   infiles      = outfiles;
   outfiles     = p1;
   outfiles->resetEntries();

   usefiles = colfiles;

   StringPipe *p3 = indata;
   indata   = outdata;
   outdata  = p3;
   outdata->resetPipe();

   usedata  = coldata;

   colfiles = 0;
   coldata  = 0;
   colbinary= 0;

   if (usefiles && usedata)
   {
      // chain command with ANY input:
      // can work only if one stream has data AND other is empty
      if (numberOfInFiles() && indata->numberOfEntries()) {
         perr("command chaining: cannot use text data AND filename list in parallel.\n");
         exit(9);
      }

      // reduce mode selection, following given data
      if (indata->numberOfEntries()) usefiles = 0;
      if (numberOfInFiles()) usedata = 0;
   }

   if (cs.tracechain)
      printf("[chain swaps out and in, text2files=%d files2text=%d]\n",text2files,files2text);

   bool bt2f = text2files;
   bool bf2t = files2text;

   text2files = 0;
   files2text = 0;

   if (bt2f && bf2t)
      return 9+perr("command chaining stopped: cannot execute text2files AND files2text\n");

   if (usedata && !usefiles && !indata->numberOfEntries()) {
      if (cs.tracechain)
         printf("[chain autoselects ftt from %d filenames.]\n",numberOfInFiles());
      bf2t = 1;
   }

   if (usefiles && !usedata && !numberOfInFiles()) {
      if (cs.tracechain)
         printf("[chain autoselects ttf from %d text lines.]\n",indata->numberOfEntries());
      bt2f = 1;
   }

   if (bf2t)
   {
      // autoconvert filename list to text, even on empty list.
      int nrec = numberOfInFiles();
      for (int i=0; i<nrec; i++) {
         Coi *pcoi = getFile(i);
         if (pcoi) indata->addEntry(pcoi->name(), str(""));
      }
      infiles->resetEntries();
      if (cs.tracechain)
         printf("[chain converts filename list to text data, %d records]\n", nrec);
      usefiles = 0;
      usedata  = 1;
   }
   else
   if (bt2f)
   {
      if (indata->numberOfEntries()) {
         // user wants to convert text to filename list
         convInDataToInFiles();
      } else if (!cs.keepchain) {
         // out of chain filenames
         if (!cs.nonotes) { // sfk1933: active by default. ignores -quiet.
            #if 1
            pinf("[nopre] no files, stopping at %s. 'sfk help chain' for options.\n",pszCmd);
            #else
            pinf("chaining stops before %s: no more filenames.\n",pszCmd);
            pinf("[nopre] ... try '+then %s', -keepchain or -nonote.\n",pszCmd);
            #endif
         }
         return 1;
      }
   }

   if (cs.tracechain) {
      if (usefiles)
         printf("[chain passes %d files to %s]\n",numberOfInFiles(),pszCmd);
      if (usedata)
         printf("[chain passes %d %s to %s]\n",
            indata->numberOfEntries(),
            nClOutBinarySize ? "records":"text lines",
            pszCmd);
   }

   nClInCheckSum = nClOutCheckSum;
   nClOutCheckSum = 0;

   if ((nClInBinarySize = nClOutBinarySize)) {
      nClOutBinarySize = 0;
      if (cs.tracechain)
         printf("[chain passes %s bytes of binary]\n", numtoa(nClInBinarySize));
   }

   return 0;
}

CommandChaining::CommandChaining()
{
   colfiles   = 0;
   usefiles   = 0;
   coldata    = 0;
   usedata    = 0;
   colbinary  = 0;

   infiles    = 0;
   outfiles   = 0;

   indata     = 0;
   outdata    = 0;
   storedata  = 0;
   perlinein  = 0;
   perlineout = 0;

   text2files = 0;
   files2text = 0;
   btold1     = 0;

   nClOutBinarySize = 0;
   nClInBinarySize = 0;
   nClOutCheckSum = 0;
   nClInCheckSum = 0;
   iClBinBufUsed = 0;
}

int CommandChaining::init()
{
   infiles  = new CoiTable();
   outfiles = new CoiTable();

   indata   = new StringPipe();
   outdata  = new StringPipe();
   storedata= new StringPipe();

   justNamesFilter = new KeyMap(); // sfk193

   // perline... stays NULL until +forline

   memset(szClPreBuf, 0, sizeof(szClPreBuf));
   memset(szClPreAttr, 0, sizeof(szClPreAttr));
   memset(szClBuf, 0, sizeof(szClBuf));
   memset(szClAttr, 0, sizeof(szClAttr));
   memset(szClBinBuf, 0, sizeof(szClBinBuf));

   nClOutBinarySize = 0;
   nClInBinarySize = 0;
   nClOutCheckSum = 0;
   nClInCheckSum = 0;
   iClBinBufUsed = 0;

   return 0;
}

void CommandChaining::reset()
{
   colfiles   = 0;
   usefiles   = 0;
   coldata    = 0;
   usedata    = 0;
   text2files = 0;
   files2text = 0;
   btold1     = 0;
   colbinary  = 0;

   infiles->resetEntries();
   outfiles->resetEntries();

   indata->resetEntries();
   outdata->resetEntries();
 
   if (justNamesFilter)
      justNamesFilter->reset(); // sfk193

   nClOutBinarySize = 0;
   nClInBinarySize = 0;
   nClOutCheckSum = 0;
   nClInCheckSum = 0;
   iClBinBufUsed = 0;
}

void CommandChaining::shutdown()
{
   if (infiles) {
      infiles->resetEntries();
      delete infiles;
   }
   if (outfiles) {
      outfiles->resetEntries();
      delete outfiles;
   }

   if (indata) {
      indata->resetEntries();
      delete indata;
   }
   if (outdata) {
      outdata->resetEntries();
      delete outdata;
   }

   if (storedata) {
      storedata->resetEntries();
      delete storedata;
   }

   if (perlinein) {
      perlinein->resetEntries();
      delete perlinein;
   }
   if (perlineout) {
      perlineout->resetEntries();
      delete perlineout;
   }

   if (justNamesFilter) { // sfk193
      justNamesFilter->reset();
      delete justNamesFilter;
   }
}

bool isDirParm(char *psz)
{
   if (!strcmp(psz, "-root"))       return true;   // sfk197
   if (!strcmp(psz, "-dir"))        return true;
   if (!strcmp(psz, "-file"))       return true;
   if (!strcmp(psz, "-fileset"))    return true;
   if (!strcmp(psz, "-using"))      return true;   // sfk1963 added
   // if (!strncmp(psz, "-from=", 6))  return true;   // sfk1963 deleted
   // if (!strcmp(psz, "-view"))       return true;   // sfk1963 deleted
   return false;
}

#ifdef VFILEBASE
extern "C" void setUzpMemLimit(num nlimit);
#endif // VFILEBASE

void setMemoryLimit(int nMBytes)
{
   num nbytes = (num)nMBytes * 1048576; // fix sfk1892 64 bits
   // no not accept limits below 10 MB:
   if (nbytes < (num)10 * 1000000) {
      perr("ignoring memlimit, illegal value: %d", nMBytes);
   } else {
      nGlblMemLimit = nbytes;
      bGlblMemLimitWasSet = 1;
   }
}

bool sfkisopt(char *psz)
{
   // sfk197: accept empty parms as -noop
   if (psz[0] == '\0') return 1;

   return strncmp(psz, "-", 1) ? 0 : 1;
}



// process -opt=value AND -opt value
bool haveParmOption(char *argv[], int argc, int &iDir, cchar *pszOptBase, char **pszOutParm)
{
   *pszOutParm = 0;  // if this stays NULL it tells ERROR status.

   // check if format -opt=value is given
   char szEqBuf[100];
   sprintf(szEqBuf, "%s=", pszOptBase);
   char *pszOpt = argv[iDir];
   if (*pszOpt == '+') pszOpt++; // e.g. +md5gento=
   if (!strncmp(pszOpt, szEqBuf, strlen(szEqBuf))) {
      *pszOutParm = pszOpt+strlen(szEqBuf);
      return 1;
   }

   // check if format "-opt value" is given
   if (!strcmp(pszOpt, pszOptBase)) {
      if (iDir >= argc-1) {
         perr("missing parameter after option %s\n", pszOptBase);
         return 0;
      }
      iDir++;  // IDIR INCREMENT IS WRITTEN BACK!
      *pszOutParm = argv[iDir];
      return 1;
   }

   return 0;
}

bool setGeneralOption(char *argv[], int argc, int &iOpt, bool bGlobal=0, bool bJustCheck=0)
{
   struct CommandStats *pcs = (bGlobal ? &gs : &cs);
   if (bJustCheck)      pcs = &dummyCommandStats;

   bool bsim = bJustCheck;

   char *psz1 = argv[iOpt];
   char *pszParm = 0;

   // sfk197: accept empty parms as general option -noop.
   if (psz1[0] == '\0') return true;

   if (!strcmp(psz1, "-stest"))     { bGlblSyntaxTest = 1; return true; }
   if (!strcmp(psz1, "-debug"))     { pcs->debug = 1; pcs->memcheck = 1; return true; }
   if (!strcmp(psz1, "-debug=2"))   { pcs->debug = 2; pcs->memcheck = 1; return true; }
   if (!strncmp(psz1, "-coi", 4))   { Coi::bClDebug = 1; return true; }
   if (!strcmp(psz1, "-noop"))      { return true; }
   if (!strcmp(psz1, "-quiet"))     { pcs->quiet = 1; return true; }
   if (!strcmp(psz1, "-quiet=2"))   { pcs->quiet = 2; return true; }
   if (!strcmp(psz1, "-quiet=3"))   { pcs->quiet = 3; return true; }
   if (!strcmp(psz1, "-stat"))      { pcs->dostat = 1; return true; } // sfk1892
   if (!strcmp(psz1, "-nostat"))    { pcs->dostat = 0; pcs->nostat=1; return true; } // sfk1892
   if (!strcmp(psz1, "-nohead"))    { pcs->nohead = 1; return true; }
   if (!strcmp(psz1, "-nocheck"))   { pcs->nocheck = 1; return true; }
   if (!strncmp(psz1, "-noinf", 6)) { pcs->noinfo = 1; return true; }
   if (!strncmp(psz1, "-nofile", 7)){ pcs->nonames = 1; return true; }
   if (!strncmp(psz1, "-noname", 7)){ pcs->nonames = 1; return true; }
   if (!strcmp(psz1,  "-subnames")) { pcs->subnames = 1; return true; }
   if (!strncmp(psz1, "-noind", 6)) { pcs->noind = 1; return true; }
   if (!strcmp(psz1, "-sim"))       { pcs->sim = 1; return true; }
   if (!strcmp(psz1, "-norec"))     { pcs->subdirs = 0; pcs->maxsub = 0; return true; }
   if (!strcmp(psz1, "-nosub2"))    { pcs->subdirs = 0; pcs->maxsub = 0; pcs->hidesubdirs = 1; return true; } // sfk1912
   if (!strncmp(psz1, "-nosub", 6)) { pcs->subdirs = 0; pcs->maxsub = 0; return true; }
   if (!strcmp(psz1, "-withsub"))   { pcs->subdirs = 1; pcs->maxsub = 1; return true; }
   if (!strcmp(psz1, "-i"))         { bGlblStdInAny = 1; return true; }
   if (!strcmp(psz1, "-verbose"))   { pcs->verbose = 1; return true; }
   if (!strcmp(psz1, "-verbose=0")) { pcs->verbose = 0; return true; }
   if (!strcmp(psz1, "-verbose=2")) { pcs->verbose = 2; return true; }
   if (!strcmp(psz1, "-verbose=3")) { pcs->verbose = 3; return true; }
   if (!strcmp(psz1, "-verbose=4")) { pcs->verbose = 4; return true; }
   if (!strcmp(psz1, "-iotrace"))   { pcs->iotrace = 1; return true; }
   if (!strcmp(psz1, "-showip"))    { pcs->showip = 1; return true; }
   if (!strcmp(psz1, "-justrc"))    { pcs->justrc = 1; return true; }
   if (strBegins(psz1, "-hid"))     { pcs->hidden = 1; return true; }
   if (strBegins(psz1, "-nohid"))   { pcs->hidden = 0; return true; }
   if (!strcmp(psz1, "-yes"))       { pcs->yes = 1; return true; }
   if (!strcmp(psz1, "-clog"))      { if (bsim) return 1; gs.logcmd = cs.logcmd = 1; return true; }
   if (!strcmp(psz1, "-yes+"))      { if (bsim) return 1; pcs->yes = 1; gs.logcmd = cs.logcmd = 1; return true; }
   if (!strcmp(psz1, "-binallchars"))  { pcs->binallchars = 1; return true; }
   if (!strcmp(psz1, "-umlauts"))   { // deprecated with sfk190
      // cs.noaccent = 0;
      return true;
   }
   if (!strcmp(psz1, "-nocasemin")) { // sfk190
      pcs->nocasemin = 1;
      return true;
   }
   if (!strcmp(psz1, "-noumlauts") || strBegins(psz1, "-noacc")) { // undocumented since sfk190
      pcs->nocasemin = 1;    // sfk190: pcs not just cs
      return true;
   }
   if (strBegins(psz1, "-utf"))     { pcs->wchardec = 1; return true; } // deprecated
   if (strBegins(psz1, "-noutf"))   { pcs->wchardec = 0; return true; } // deprecated
   if (strBegins(psz1, "-wchar"))   { pcs->wchardec = 1; return true; }
   if (strBegins(psz1, "-nowchar")) { pcs->wchardec = 0; return true; }
   if (!strcmp(psz1, "-nocol"))     { pcs->usecolor = pcs->usehelpcolor = 0; return true; }
   if (!strcmp(psz1, "-col"))       { pcs->usecolor = 1; return true; }
   if (!strcmp(psz1, "-case"))      { pcs->usecase = 1; return true; }
   if (!strcmp(psz1, "-nocase"))    { pcs->usecase = 0; pcs->nocase = 1; return true; }
   if (!strcmp(psz1, "-withdirs"))  { pcs->withdirs = 1; return true; }
   if (!strncmp(psz1, "-wdir", 5))  { pcs->withdirs = 1; return true; }
   if (!strcmp(psz1, "-justdirs"))  { pcs->justdirs = 1; return true; }
   if (!strcmp(psz1, "-names"))     { pcs->useJustNames = 1; return true; }
   if (!strcmp(psz1, "-names2"))    { pcs->useJustNames = 2; return true; }
   if (!strcmp(psz1, "-notnames"))  { pcs->useNotNames = 1; return true; }
   if (!strncmp(psz1, "-rel", 4))   { pcs->rootrelname = 1; return true; }
   if (!strncmp(psz1, "-absname", 8)) { pcs->rootabsname = 1; return true; }
   if (!strncmp(psz1, "-quot", 5))  { bGlblQuoted = 1; return true; }
   if (!strcmp(psz1, "-nocconv"))   { pcs->outcconv = 0; return true; }
   if (!strcmp(psz1, "-cconv"))     { pcs->forcecconv = 1; return true; }
   if (!strcmp(psz1, "-incref"))    { bGlblSinceDirIncRef = 1; return true; }
   if (!strcmp(psz1, "-force"))     { pcs->force = 1; return true; }
   if (!strcmp(psz1, "-force=2"))   { pcs->force = 2; return true; }
   if (!strncmp(psz1, "-lit", 4))   { pcs->spat = 0; pcs->wpat = 0; return true; }
   if (!strcmp(psz1, "-spats"))     { pcs->spat = 2; return true; }
   if (!strncmp(psz1, "-spat", 5))  { pcs->spat = 1; return true; }
   if (!strncmp(psz1, "-nospat", 7)){ pcs->spat = 0; return true; }
   #ifndef USE_SFK_BASE // sfk180
    // cmod text_match
    #if (sfk_prog || sfk_text_match)
    if (!strncmp(psz1, "-xpat", 5))  { pcs->xpat = 1; return true; }
    if (strBegins(psz1, "-xmaxlen=")) {
      if (bsim) return 1;
      SFKMatchDefaultMaxLen = (int)numFromSizeStr(psz1+9, psz1);
      if (SFKMatchDefaultMaxLen < 1)
         { perr("invalid -xmaxlen"); exit(9); }
      return true;
    }
    // emod text_match
    #endif // (sfk_prog || sfk_text_match)
   #endif // USE_SFK_BASE
   if (!strcmp(psz1, "-bright"))    { nGlblDarkColBase = 1; return true; }
   if (!strcmp(psz1, "-dark"))      { nGlblDarkColBase = 0; return true; }
   if (!strcmp(psz1, "-nochain"))   { pcs->nochain = 1; return true; }
   if (!strcmp(psz1, "-showrc"))    { if (bsim) return 1; gs.showrc = 1; return true; }
   if (!strcmp(psz1, "-exectime"))  { if (bsim) return 1; gs.tellExecTime = 1; return true; }
   if (!strncmp(psz1, "-nowarn", 7)) { pcs->nowarn = 1; return true; }
   if (!strncmp(psz1, "-noerr",  6)) { pcs->noerr = 1; return true; }
   if (!strncmp(psz1, "-errtotext", 6)) { pcs->errtotext = 1; return true; }
   if (!strncmp(psz1, "-nonote", 7)) { pcs->nonotes = 1; return true; }
   if (!strncmp(psz1, "-nofo", 5))   { pcs->skipLinks = 1; return true; }
   if (!strncmp(psz1, "-rawflags", 5)) { pcs->traceFileFlags = 1; return true; }
   if (!strncmp(psz1, "-ltarg", 6))  { pcs->listTargets = 1; return true; }
   if (!strncmp(psz1, "-stoponerr", 10))  { pcs->treeStopRC = 9; return true; }
   if (!strncmp(psz1, "-echoonerr", 11)) { pcs->echoonerr = 1; return true; }
   if (!strncmp(psz1, "-rcfromerr", 10))  { pcs->rcFromError = 9; return true; }
   if (!strncmp(psz1, "-waitonerr", 10))  { bGlblPauseOnError = 1; return true; }
   else
   if (!strncmp(psz1, "-wait", 5))  { bGlblPauseOnEnd = 1; return true; }
   if (!strncmp(psz1, "-exterr", 7)){ bGlblSysErrDetail = 1; return true; }
   if (!strncmp(psz1, "-detail", 7)){ bGlblSysErrDetail = 1; return true; }
   if (!strcmp(psz1, "-showskip"))  { pcs->showdupdirs = 1; return true; }
   if (strBegins(psz1, "-allowdup")) { pcs->usecirclemap = 0; return true; }
   if (!strcmp(psz1, "-fast"))      { pcs->fast = 1; return true; }
   if (strBegins(psz1, "-nover"))   { pcs->verify = 0; return true; }
   if (strBegins(psz1, "-verify"))  { pcs->verify = 1; return true; }
   if (!strcmp(psz1, "-noprog"))    { pcs->noprog = 1; return true; }
   if (!strcmp(psz1, "-test"))      { pcs->test = 1; return true; }
   if (!strcmp(psz1, "-oldmd5"))    { bGlblOldMD5 = 1; return true; }
   if (strBegins(psz1, "-withbin"))    { pcs->textfiles = 0; pcs->binaryfiles = 0; return true; }
   if (strBegins(psz1, "-textandbin")) { pcs->textfiles = 0; pcs->binaryfiles = 0; return true; }
   if (strBegins(psz1, "-text"))       { pcs->textfiles = 1; return true; }
   if (strBegins(psz1, "-textfile"))   { pcs->textfiles = 1; return true; }
   if (strBegins(psz1, "-nobin"))      { pcs->textfiles = 1; return true; }
   if (strBegins(psz1, "-bin"))        { pcs->binaryfiles = 1; return true; }
   if (strBegins(psz1, "-binfile"))    { pcs->binaryfiles = 1; return true; }
   if (strBegins(psz1, "-binaryfile")) { pcs->binaryfiles = 1; return true; }
   if (!strcmp(psz1, "-keepdate"))  { pcs->keeptime = 1; return true; }
   if (!strcmp(psz1, "-keeptime"))  { pcs->keeptime = 1; return true; }
   if (!strcmp(psz1, "-noesckey"))  { bGlblDisableEscape = 1; return true; }
   if (!strcmp(psz1, "-snap"))      { pcs->usesnap = 1; return true; }
   if (!strncmp(psz1, "-snapw", 6)) { // snapwithnames
      pcs->usesnap = 1;
      pcs->usesnapfiltname = 1;
      return true;
   }
   if (!strcmp(psz1, "-upat")) {
      setLinuxSyntax(1); // -upat
      return true;
   }
   if (!strcmp(psz1, "-upat2")) {
      setLinuxSyntax(2); // -upat2
      return true;
   }
   if (!strcmp(psz1, "-upath")) { pcs->upath = 1; return true; }
   #ifdef VFILEBASE
   if (!strcmp(psz1, "-qarc") || !strcmp(psz1, "-qxarc")) {
      pcs->travelzips  = 1;
      pcs->probefiles  = (strcmp(psz1, "-qxarc") ? 0 : 1);
      pcs->xelike      = 1;
      pcs->shallowzips = 1;  // toplevel only
      return true;
   }
   else
   #endif // VFILEBASE
   if (!strcmp(psz1, "-qarc") || !strcmp(psz1, "-qxarc")) {
      pcs->travelzips  = 1;
      pcs->probefiles  = (strcmp(psz1, "-qxarc") ? 0 : 1);
      return true;
   }
   if (!strcmp(psz1, "-zip") || !strcmp(psz1, "-arc") || !strcmp(psz1, "-xarc")) {
      if (!strcmp(psz1, "-zip"))
         pwarn("option -zip is deprecated, use -arc instead."); // sfk 175
      pcs->travelzips  = 1;
      pcs->probefiles  = (strcmp(psz1, "-xarc") ? 0 : 1);
      #ifdef VFILEBASE
      pcs->xelike     = 1;  // process zips as deep as possible
       #ifdef USE_SFK_BASE
       pcs->precachezip = 1; // dv load: always force precache
       #endif // USE_SFK_BASE
      #endif // VFILEBASE
      return true;
   }
   if (!strcmp(psz1, "-nozip") || !strcmp(psz1, "-noarc"))
      { pcs->travelzips = 0; return true; }

   #ifdef VFILEBASE
   if (!strcmp(psz1, "-extdom"))    { pcs->extdomref = 1; return true; }
   if (!strcmp(psz1, "-xd"))        { pcs->xelike    = 1; return true; }
   if (!strcmp(psz1, "-cacheall"))  { pcs->cacheall  = 1; return true; }
   if (!strcmp(psz1, "-cachestat")) { if (bsim) return 1; gs.cachestat = 1; return true; }
   if (!strcmp(psz1, "-nocache"))   { setDiskCacheActive(0); return true; }
   #endif // VFILEBASE

   if (strBegins(psz1, "-noipex"))  { pcs->noipexpand = 1; return true; }
   if (strBegins(psz1, "-crashtest"))  { pcs->crashtest = 1; return true; }

   // -pure is often a local option, but some commands allow general use:
   if (bGlblAllowGeneralPure && !strcmp(psz1, "-pure"))
      { pcs->pure = 1; return true; }

   if (!strcmp(psz1, "-allbin") || !strcmp(psz1, "-include-all-binaries"))
      { pcs->incbin = 1; return true; }

   if (!strcmp(psz1, "-xbin") || !strcmp(psz1, "-wlbin")) // sfk1944 internal
      { pcs->incwlbin = 1; return true; }

   if (!strcmp(psz1,"-wrap") || !strcmp(psz1,"-rewrap"))
   {
      if (!strcmp(psz1,"-rewrap")) pcs->rewrap = 1;
      // wrap with auto-calculated number of columns
      int nCols = autoCalcWrapColumns();
      if (nCols) {
         pcs->wrapcol = nCols;
         if (pcs->wrapbincol == 80) // if on default
            pcs->wrapbincol = nCols;
      }
      mtklog(("opt: done %s, wrapcol=%d, wrapbincol=%d",psz1,pcs->wrapcol,pcs->wrapbincol));
      return true;
   }
   if (strBegins(psz1,"-wrap=") || strBegins(psz1,"-rewrap="))
   {
      int nCols = 0;
      if (strBegins(psz1,"-rewrap=")) {
         pcs->rewrap = 1;
         nCols = atol(psz1+8);
      } else {
         nCols = atol(psz1+6);
      }
      // wrap with fixed (user-defined) number of columns
      if (nCols) {
         pcs->wrapcol = nCols;
         if (pcs->wrapbincol == 80) // if on default
            pcs->wrapbincol = nCols;
      }
      mtklog(("opt: done %s, wrapcol=%d, wrapbincol=%d",psz1,pcs->wrapcol,pcs->wrapbincol));
      return true;
   }
   if (!strncmp(psz1,"-wrapbin",8)) {
      // wrap for text extracted from binary files
      int nCols = 0;
      if (!strncmp(psz1,"-wrapbin=",9))
         nCols = atol(psz1+9);
      else
         nCols = autoCalcWrapColumns();
      if (nCols) {
         pcs->wrapbincol = nCols;
      }
      return true;
   }
   if (!strcmp(psz1, "-memcheck"))  { pcs->memcheck = 1; return true; }
   #ifdef SFK_MEMTRACE
   if (!strcmp(psz1, "-nomemcheck")) {
      bGlblNoMemCheck = 1;
      sfkmem_nocheck();
      return true;
   }
   #endif // SFK_MEMTRACE
   if (strBegins(psz1, "-memlimit=")) {
      int nMBytes = atol(psz1+10);
      setMemoryLimit(nMBytes);
      return true;
   }
   if (strBegins(psz1, "-weblimit=")) {
      int nMBytes = atol(psz1+10);
      pcs->maxwebsize = nMBytes * 1000000;
      return true;
   }
   if (!strncmp(psz1,"-keepstale=",strlen("-keepstale="))) {
      nGlblActiveFileAgeLimit = atol(psz1+strlen("-keepstale="));
      return true;
   }
   if (!strncmp(psz1, "-html", 5)) {
      bGlblHtml = 1;
      if (!getenv("SFK_COLORS"))
         setColorScheme("file:1,head:4,examp:8");
      if (!strcmp(psz1, "-htmlpage"))
         printf("<font face=\"courier\" size=\"2\"><pre>\n");
      return true;
   }
   if (   !strcmp(psz1, "-sincedir") || !strcmp(psz1, "-sd")
       || !strcmp(psz1, "-sinceadd") || !strncmp(psz1, "-sincedif", 9)
       || !strncmp(psz1, "-sincech", 8) || !strcmp(psz1, "-sc")
      )
   {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char *psz2 = argv[iOpt];
      if (*psz2 == '-') { perr("need directory name, no option allowed after %s\n", psz1); exit(9); }
      pszGlblSinceDir = strdup(psz2);
      if (!bGlblSyntaxTest && !isDir(pszGlblSinceDir))
         { perr("no such directory: %s\n", pszGlblSinceDir); exit(9); }
      if (!strcmp(psz1, "-sinceadd"))
         nGlblSinceMode |= 1;
      else
      if (!strncmp(psz1, "-sincedif", 9))
         nGlblSinceMode |= 2;
      else
      if (!strncmp(psz1, "-sincech", 8) || !strcmp(psz1, "-sc"))
         nGlblSinceMode |= 1+2;  // only add and dif, no time diff
      else
         nGlblSinceMode = 1+2+4; // default: list all differences
      return true;
   }
   if (!strncmp(psz1, "-ignoretime", 11)) {
      bGlblIgnoreTime = 1;
      return true;
   }
   if (!strcmp(psz1, "-ignore3600")) {
      bGlblIgnore3600 = 1;
      return true;
   }
   if (!strcmp(psz1, "-noignore3600")) {
      bGlblIgnore3600 = 0;
      return true;
   }
   if (!strncmp(psz1, "-tracesel", 9)) {
      nGlblTraceSel |= 3;
      return true;
   }
   if (!strncmp(psz1, "-tracedir", 9)) {
      nGlblTraceSel |= 1;
      return true;
   }
   if (!strncmp(psz1, "-tracefile", 10)) {
      nGlblTraceSel |= 2;
      return true;
   }
   if (!strcmp(psz1, "-tracechain"))
      { pcs->tracechain = 1; return true; }
   if (!strcmp(psz1, "-usectime")) {
      pcs->usectime = 1;
      return true;
   }
   if (!strcmp(psz1, "-utc") || !strcmp(psz1, "-gmt")) {
      pcs->useutc = 1;
      return true;
   }
   #ifdef VFILENET
   if (!strcmp(psz1, "-proxy")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char szBuf[200];
      char *pproxy = argv[iOpt];
      strcopy(szBuf, pproxy);
      char *psz1 = szBuf;
      while (*psz1 && *psz1 != ':') psz1++;
      if (*psz1) *psz1++ = '\0';
      int nport = atol(psz1);
      if (!nport) nport = 80;
      TCPCore::setProxy(szBuf, nport); // -proxy option
      return true;
   }
   #endif // VFILENET
   if (strBegins(psz1, "-minsize=")) {
      pcs->minsize = numFromSizeStr(psz1+9, psz1);
      if (pcs->minsize < 0) exit(9);
      return true;
   }
   if (strBegins(psz1, "-maxsize=")) {
      pcs->maxsize = numFromSizeStr(psz1+9, psz1);
      if (pcs->maxsize < 0) exit(9);
      return true;
   }
   if (!strcmp(psz1, "-since")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char *psz2 = argv[iOpt];
      if (tryGetRelTime(psz2, pcs->sincetime))
      { }
      else
      if (timeFromString(psz2, pcs->sincetime)) // local option -since
         exit(9);
      if (pcs->untiltime && pcs->untiltime <= pcs->sincetime)
         {  perr("-before lower or equal than -since was specified.\n"); exit(9); }
      return true;
   }
   if (!strcmp(psz1, "-before")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char *psz2 = argv[iOpt];
      if (tryGetRelTime(psz2, pcs->untiltime))
      { }
      else
      if (timeFromString(psz2, pcs->untiltime)) // local option -before
         exit(9);
      if (pcs->sincetime && pcs->sincetime >= pcs->untiltime)
         {  perr("-since greater or equal than -before was specified.\n"); exit(9); }
      return true;
   }
   if (!strcmp(psz1, "-to")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      cs.tomask = argv[iOpt];
      cs.tomaskfile = 0;
      return true;
   }
   if (!strcmp(psz1, "-tofile")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      cs.tomask = argv[iOpt];
      cs.tomaskfile = 1;
      return true;
   }
   if (!strcmp(psz1, "-todir")) { // sfk200 generic -todir, local versions exist
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      snprintf(cs.mskbuf, sizeof(cs.mskbuf)-10, "%s%c%crelfile",
         argv[iOpt], glblPathChar, glblRunChar);
      cs.tomask = cs.mskbuf;
      cs.tomaskfile = 0;
      return true;
   }
   if (!strcmp(psz1, "-tomake")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      bGlblGotToMake = 1;
      char *pszMask = argv[iOpt];
      // auto extend .mp4 as $path\$base.mp4
      if (!strchr(pszMask, glblRunChar) && !isalnum(pszMask[0])
          && !strchr(pszMask, glblPathChar)) {
         snprintf(cs.tomake,sizeof(cs.tomake)-10,
            "%cpath%c%cbase%s",
            glblRunChar, glblPathChar, glblRunChar, pszMask);
         if (cs.verbose)
            printf("[extended -tomake as %s]\n", cs.tomake);
      } else {
         strcopy(cs.tomake, pszMask);
      }
      return true;
   }
   if (!strcmp(psz1, "-tmpdir")) {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      SFTmpFile::setTmpDir(argv[iOpt]);
      return true;
   }
   if (!strcmp(psz1, "-keeptmp")) {
      cperm.keeptmp = 1;
      return true;
   }
   if (!strcmp(psz1, "-showtmp")) {
      cperm.showtmp = 1;
      return true;
   }
   if (!strcmp(psz1, "-today")) {
      tryGetRelTime("today", pcs->sincetime);
      return true;
   }
   if (!strcmp(psz1, "-flist") || !strcmp(psz1, "-fl"))
   {
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char *pszFile = argv[iOpt];

      // read list of filenames from a file.
      Coi ocoi(pszFile, 0);
      if (ocoi.open("rb"))
         { perr("cannot read filename list file: %s\n", pszFile); exit(9); }
      while (ocoi.readLine(szLineBuf, sizeof(szLineBuf)-10) > 0)
      {
         szLineBuf[sizeof(szLineBuf)-10] = '\0';
         removeCRLF(szLineBuf);
         if (cs.debug) printf("] add2flist.1: %s\n", szLineBuf);
         Coi ocoisub(szLineBuf, 0);
         glblSFL.addEntry(ocoisub); // is copied
      }
      ocoi.close();

      cs.usingflist = 1;

      // if no further dir parms follow, do not autocomplete.
      cs.blockAutoComplete = 1;

      return true;
   }
   if (!strcmp(psz1, "-ilist")) // sfk200 internal
   {
      if (bsim) return 1;
      myfgets_init(1); // use stdin
      while (myfgets(szLineBuf, MAX_LINE_LEN))
      {
         removeCRLF(szLineBuf);
         Coi ocoisub(szLineBuf, 0);
         glblSFL.addEntry(ocoisub); // is copied
      }

      cs.usingflist = 1;

      // if no further dir parms follow, do not autocomplete.
      cs.blockAutoComplete = 1;

      return true;
   }
   #ifdef SFK_LINUX_FULL
   if (!strncmp(psz1,"-prio=", 6)) {
      setPriority(atol(psz1+6));
      return true;
   }
   #endif
   if (!strncmp(psz1,"-delay=",strlen("-delay="))) {  // sfk1982
      pcs->postFileDelay = atol(psz1+strlen("-delay="));
      return true;
   }
   if (!strncmp(psz1,"-predelay=",strlen("-predelay="))) { // internal
      pcs->preFileDelay = atol(psz1+strlen("-predelay="));
      return true;
   }
   if (!strncmp(psz1,"-dirdelay=",strlen("-dirdelay="))) {
      pcs->walkDirDelay = atol(psz1+strlen("-dirdelay="));
      return true;
   }
   if (!strncmp(psz1,"-filedelay=",strlen("-filedelay="))) { // internal
      pcs->preFileDelay = atol(psz1+strlen("-filedelay="));
      return true;
   }
   if (!strcmp(psz1,"-slow=3"))   {
      pcs->walkDirDelay  = 10;
      pcs->preFileDelay = 10;
      return true;
   }
   if (!strcmp(psz1,"-slow=2"))   {
      pcs->walkDirDelay  = 100;
      return true;
   }
   if (!strcmp(psz1,"-slow")) { // sfk1952
      #ifdef _WIN32
      SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
      #endif
      #ifdef SFK_LINUX_FULL
      setPriority(-1);
      #endif
      return true;
   }
   if (!strcmp(psz1,"-broad"))      {  pcs->incFNameInPath = 1; return true; }   // sfk187 global support
   if (!strcmp(psz1,"-firsthit"))   {  pcs->useFirstHitOnly = 1; return true; }  // sfk187 global support
   if (!strcmp(psz1,"-xchars"))     {  pcs->xchars = 1; return true; }
   if (!strcmp(psz1,"-perf"))       {  pcs->perf = 1; return true; }
   if (!strcmp(psz1,"-crlf"))       {  strcpy(pcs->szeol, "\r\n"); return true; }
   if (!strcmp(psz1,"-lf"))         {  strcpy(pcs->szeol, "\n"); return true; }
   if (strBegins(psz1,"-toiso="))   {  pcs->toiso = 1; pcs->toisodef = psz1[strlen("-toiso=")]; return true; }
   if (!strcmp(psz1,"-toiso"))      {  pcs->toiso = 1; return true; }
   if (!strcmp(psz1,"-iso"))        {  pcs->toiso = 1; return true; }
   if (!strcmp(psz1,"-toutf"))      {  pcs->toutf = 1; return true; }
   if (!strcmp(psz1,"-toutfsafe"))  {  pcs->toutf = 2; return true; }
   if (!strcmp(psz1,"-uname"))      {
      pcs->uname = 1; pcs->outcconv = 0;
      return true;
   }
   #ifdef SFK_UNAME
   if (!strcmp(psz1,"-tname"))      {
      pcs->tname = 1; pcs->outcconv = 0;
      return true;
   }
   #endif // SFK_UNAME
   // TODO: -utf name conflict with experimental utf16 decode
   #ifdef VFILEBASE
   if (strBegins(psz1,"-useragent="))
   {
      setHTTPUserAgent(psz1+strlen("-useragent="));
      return true;
   }
   #endif // VFILEBASE
   if (!strcmp(psz1, "-rawhelp")) {
      bGlblDumpHelpRaw=1;
      return true;
   }
   if (!strncmp(psz1,"-webtimeout=",12)) {
      pcs->maxwebwait = atol(psz1+12);
      return true;
   }
   if (!strcmp(psz1,"-webnoclose")) {  // sfk198 internal
      pcs->webnoclose = 1;
      return true;
   }
   if (!strncmp(psz1,"-ftptimeout=",12)) {
      pcs->maxftpwait = atol(psz1+12);
      return true;
   }
   // --- user/pw generic options begin ---
   if (haveParmOption(argv, argc, iOpt, "-user", &pszParm)) {
      if (!pszParm) return 9;
      pcs->puser = pszParm;
      return true;
   }
   if (haveParmOption(argv, argc, iOpt, "-pw", &pszParm)) {
      if (!pszParm) return 9;
      pcs->ppass = pszParm;
      return true;
   }
   // global option e.g. for filter http://
   if (haveParmOption(argv, argc, iOpt, "-webuser", &pszParm)) {
      if (!pszParm) return 9;
      pcs->pwebuser = pszParm;
      return true;
   }
   if (haveParmOption(argv, argc, iOpt, "-webpw", &pszParm)) {
      if (!pszParm) return 9;
      pcs->pwebpass = pszParm;
      return true;
   }
   // --- user/pw generic options end ---
   if (!strcmp(psz1, "-qraw"))    { pcs->mlquotes = 'r'; return true; }
   if (!strcmp(psz1, "-qtrim"))   { pcs->mlquotes = 'f'; return true; }
   if (!strcmp(psz1, "-headers")) { pcs->showhdr  = 255; return true; }
   if (!strcmp(psz1, "-header") || !strcmp(psz1, "-head")) { // sfk1972
      ++iOpt;
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      char *ptoadd = argv[iOpt];
      int   nadd = (int)strlen(ptoadd);
      if (pcs->headers == 0) {
         pcs->headers = new char[nadd+20];
         if (!pcs->headers) { perr("outofmem"); return false; }
         snprintf(pcs->headers, nadd+10, "\n%s\r\n", ptoadd);
      } else {
         char *pold = pcs->headers;
         int   nold = (int)strlen(pold);
         pcs->headers = new char[nold+nadd+20];
         if (!pcs->headers) { perr("outofmem"); return false; }
         memcpy(pcs->headers, pold, nold);
         memcpy(pcs->headers+nold, ptoadd, nadd+1);
         strcpy(pcs->headers+nold+nadd, "\r\n");
      }
      return true;
   }
   if (!strcmp(psz1, "-showreq")) { pcs->showreq  =   1; return true; }
   // for global setting. also interpreted in SFKMapArgs.
   if (!strcmp(psz1, "-novar"))   { pcs->usevars  = 0; return true; }
   if (!strcmp(psz1, "-var"))     { pcs->usevars  = 1; return true; }
   if (!strcmp(psz1, "-strict"))    { pcs->strict = 1; return true; }
   if (!strcmp(psz1, "-nostrict"))  { pcs->strict = 0; return true; }
   if (!strcmp(psz1, "-nopass"))    { pcs->nopass = 1; return true; }
   if (!strcmp(psz1, "-strictif"))  { pcs->strictif = 1; return true; } // sfk1990

   // sfk187
   if (!strcmp(psz1, "-chainweb") || !strcmp(psz1, "-cweb"))
      { if (bsim) return 1; gs.cweb = 1; return true; }
   if (!strcmp(psz1, "-nochainweb") || !strcmp(psz1, "-nocweb"))
      { if (bsim) return 1; gs.cweb = 0; return true; }

   if (!strcmp(psz1, "-deacc"))      { pcs->fuzz = 1; return true; }
   if (!strcmp(psz1, "-tracecase"))  { pcs->tracecase = 1; return true; }
   if (!strcmp(psz1, "-tracecase2")) { pcs->tracecase = 2; return true; }
   if (!strcmp(psz1, "-isochars") || !strcmp(psz1, "-isoc"))   {
      sfkchars.setacp(1252);
      sfkchars.setocp(850);
      return true;
   }
   if (!strncmp(psz1,"-codepage=",10)) {
      char *psz=psz1+10;
      int iacp = atoi(psz);
      if (sfkchars.setacp(iacp)) {
         perr("unsupported ansi codepage: %d\n", iacp);
         return false;
      }
      int iocp = 0;
      while (*psz!=0 && isdigit(*psz)!=0) psz++;
      if (*psz=='-') psz++;
      if (isdigit(*psz)) {
         iocp = atoi(psz);
         if (sfkchars.setocp(iocp)) {
            perr("unsupported OEM codepage: %d\n", iocp);
            return false;
         }
      }
      if (gs.tracecase || cs.tracecase) {
         sfkchars.init();
         printf("[using Ansi codepage %u, OEM codepage %u]\n",
            sfkchars.iclacp, sfkchars.iclocp);
      }
      return true;
   }
   if (!strcmp(psz1, "-more"))
      { pcs->nmore = nGlblConsRows; return true; }
   if (strBegins(psz1, "-more") && isdigit(psz1[5])!=0)
      { pcs->nmore = atoi(psz1+5); return true; }
   #ifdef SFKOFFICE
   // if (!strcmp(psz1, "-office2")) { pcs->office = 2; return true; }
   // if (!strcmp(psz1, "-office"))  { pcs->office = 1; return true; }
   // sfk1944: -notext was an undocumented, unused option.
   if (!strcmp(psz1, "-notext"))     { return true; }
   // sfk1944 limit just to office files with some commands.
   if (!strcmp(psz1, "-justoffice") || !strcmp(psz1, "-justdoc"))
      { pcs->justoffice = 1; return true; }
   if (!strcmp(psz1, "-crcmd5")) // internal, LOCAL only
      { cs.crcmd5 = 1; return true; }
   if (!strncmp(psz1, "-crc=", 5)) {
      cs.bjustcrc = 1;
      cs.njustcrc = strtoul(psz1+5, 0, 0x10);
      return true;
   }
   #endif // SFKOFFICE
   if (!strcmp(psz1, "-keepdata"))   { pcs->keepdata = 1; return true; }
   if (!strcmp(psz1, "-keepchain"))  { pcs->keepchain = 1; return true; }
   if (!strcmp(psz1, "-stoponempty")) { pcs->keepchain = 0; return true; } // sfk1990
   if (!strncmp(psz1, "-every=", 7)) { pcs->justevery = atoi(psz1+7); return true; }
   if (!strncmp(psz1, "-cliptries=", 11)) { pcs->cliptries = atoi(psz1+11); return true; }
   /*
   if (!strcmp(psz1, "-root")) { // done in pdp
      // this option takes another parameter!
      ++iOpt;  // new iOpt IS WRITTEN BACK.
      if (bsim) return 1;
      if (iOpt >= argc) { perr("missing parameter after %s\n", psz1); exit(9); }
      strcopy(cs.rootdir, argv[iOpt]);
      return true;
   }
   */
   if (!strcmp(psz1, "-checkdir")
       || !strcmp(psz1, "-checkdirs")) // sfk197
         { pcs->checkdirs = 1; return true; }
   if (!strcmp(psz1, "-noqwild"))  { pcs->noqwild = 1; return true; }

   bool bdonechan = 0;
   if (!strncmp(psz1,"-chan=",6))      // sfk1972
      { pcs->chan = atol(psz1+6); bdonechan=1; }
   if (!strncmp(psz1,"-channel=",9))
      { pcs->chan = atol(psz1+9); bdonechan=1; }
   if (!strncmp(psz1,"-servchan=",10))      // sfk1972
      { pcs->chanserv = atol(psz1+10); bdonechan=1; }
   if (!strncmp(psz1,"-servchannel=",13))
      { pcs->chanserv = atol(psz1+13); bdonechan=1; }
   if (bdonechan) {
      if (pcs->chan<1 || pcs->chan>=20
          || pcs->chan<1 || pcs->chan>=20)
         perr("invalid channel");
      else
         return true;
   }

   if (!strncmp(psz1, "-maxsub=", 8)) {
      pcs->subdirs = 1;
      pcs->maxsub = atoi(psz1+8);
      // -maxsub=0 intention is -nosub
      if (pcs->maxsub == 0)
         pcs->subdirs = 0;
      return true;
   }

   return false;
}

int processDirParms(char *pszCmd, int argc, char *argv[], int iDir, int nAutoComplete, int *iDirNext=0, bool *pAnyDone=0);

void skipSpaceRem(char **pszInOut, bool bUseUsingRemarks)
{
   static bool btold=0;

   char *psz1 = *pszInOut;

   // skip all whitespaces and remark lines
   while (*psz1)
   {
      if (*psz1==' ' || *psz1=='\t' || *psz1=='\r' || *psz1=='\n')
         { psz1++; continue; }

      if (bUseUsingRemarks)
      {
         // -using remarks start '//' only
         if (!strncmp(psz1, "//", 2) && isspace(psz1[2]))
         {
            while (*psz1 && *psz1 != '\n') psz1++;
            if (*psz1) psz1++; // skip LF
            continue;
         }
      }
      else
      {
         // -fileset remarks start '#'
         if (*psz1=='#'
             || *psz1==';' // sfk197 deprecated, undocumented
             )
         {
            if (*psz1==';' && btold==0) {
               btold=1;
               pwarn("fileset remarks starting ';' are deprecated, use '#': %.32s\n", dataAsTrace(psz1));
            }
            while (*psz1 && *psz1!='\n') psz1++;
            if (*psz1) psz1++; // skip LF
            continue;
         }
      }

      break;
   }

   *pszInOut = psz1;
}

int setFileListMode(int iListMode, int bDirMaskAndMatch, int bIncFNameInPath)
{
   nGlblListMode = iListMode;
   cs.dirMaskAndMatch = bDirMaskAndMatch;
   cs.incFNameInPath  = bIncFNameInPath;
   return 0;
}

// uses szLineBuf. PreCmd is optional prefix, usually "-any" or NULL.
int processFlatDirParms(char *pszPreCmd, char *pszCmdLine, int nAutoComplete)
{
   if (cs.debug) printf("processFlatDirParms\n");

   // make copy of input to allow overwriting.
   strncpy(szLineBuf, pszCmdLine, sizeof(szLineBuf)-10);
   szLineBuf[sizeof(szLineBuf)-10] = '\0';
   pszCmdLine = szLineBuf;
 
   pGlblFileParms = new StringTable(); // processFlatDirParms

   if (pszPreCmd)
      pGlblFileParms->addEntry(pszPreCmd);

   int nAbsPathParms = 0;

   char *psz1 = pszCmdLine;
   while (*psz1)
   {
      // find next token.
      skipSpaceRem(&psz1, 0); // processFlat
 
      // find end of token. support "parm with blanks".
      char *psz2 = psz1+1;
      if (*psz1 == '"')
         while (*psz2 && *psz2!='"')
            psz2++;
      while (*psz2 && *psz2!=' ' && *psz2!='\t' && *psz2!='\r' && *psz2!='\n')
         psz2++;

      // isolate token
      if (*psz2)
         *psz2++ = '\0';

      // line-end may lead to empty entry, therefore
      if (strlen(psz1)) // only if not line-end
      {
         // strip "", if any
         if (*psz1 == '"' && strlen(psz1) >= 2) {
            psz1++;
            int nLen = strlen(psz1);
            if (psz1[nLen-1] == '"')
                psz1[nLen-1] = '\0';
         }
         if (cs.debug) printf("token: <<%s>>\n", psz1);
         pGlblFileParms->addEntry(psz1);

         // count number of parms starting absolute
         if (isAbsolutePath(psz1))
            nAbsPathParms++;

         #ifdef VFILEBASE
         Coi ocoi(psz1, str(""));
         if (maskEndsWithArcExt(psz1, 4) || ocoi.isZipSubEntry())
            setArcTravel(1, 1, 2);
         #endif // VFILEBASE
      }

      // continue with next token, if any
      psz1 = psz2;
   }

   // pGlblFileParms now holds all parms
   int nParms = pGlblFileParms->numberOfEntries();
   mtklog(("pfdp nparms %d", nParms));
   apGlblFileParms = new char*[nParms];
   for (int i=0; i<nParms; i++) {
      char *pszParm = pGlblFileParms->getEntry(i, __LINE__);
      apGlblFileParms[i] = pszParm;
      mtklog(("   copied %s", pszParm ? pszParm : "<null>"));
   }
   nGlblFileParms = nParms;

   // if user drops a bunch of stuff onto the .exe
   if ((nGlblFileParms > 0) && (nGlblFileParms == nAbsPathParms)) {
      // then we don't process "dir .ext1 .ext2" but a mixed list
      bGlblHaveMixedDirFileList = 1;
   }

   return processDirParms(str(""), nParms, apGlblFileParms, 0, nAutoComplete);
}

enum ePDPStates
{
   eST_Idle       = 0,
   eST_RootDirs   = 1,
   eST_FileMasks  = 2,
   eST_GrepPat    = 3,
   eST_IncBin     = 4,
   eST_DirFile    = 5,
   eST_SubDirs    = 6,

   eST_MAXStates
};

cchar *aGlblChainCmds[] =
{
   // list of all chainable commands and their name variations
   // (first unambigious chars). every command is prefixed
   // by the default input data type (1:filenames 2:text)

   "2filt",         // receive+send files and TEXT
   "1ffilt",        // receive+send FILES and text
   "1filefilt",     // receive+send FILES and text
   "1list",         // receive+send FILES and text
   "8sel",          // receive nothing, force flush. sfk181

   "1md5gento",     // receive+pass files
   "1snapto",       // receive+pass files
   "1scantab",      // receive+send files
   "2detab",        // receive+send files and TEXT
   "1entab",        // receive+send FILES [and text]
   "1lf-to-crlf","1crlf-to-lf","1addcr","1remcr",
   "1find","1grep", // receive+send FILES and text
   "1ftext",        // receive+send FILES and text
   "1run",          // receive+send FILES and text
   "1rerun",        // receive+send FILES and text
   "1inst",         // receive files
   "1deblank",      // receive+send files
   "0noop",
   "0begin",        // sfk189
   "0endif",        // sfk189
   "6replytcp",     // receive text and binary
   "1rep",          // receive+send files
   "1xrep",         // receive+send files sfk1840 with base/xd
   "1extract",      // receive+send files sfk1840 with base
   "1xhex",         // receive+send files
   "1hexfind",      // receive+send files
   "2view",         // receive files and TEXT
   "1fv",           // receive FILES and text (fview)
   "2vc",           // receive files and TEXT
   "1del",          // receive FILES and text
   "1rmtree",       // receive FILES and text
   "0sleep",        // pass-thru text
   "0label",        // pass-thru text
   "3tail",         // receive ANY
   "3head",         // receive ANY
   "2ttail",        // receive TEXT
   "2thead",        // receive TEXT
   "6toclipw",      // receive binary sfk1841
   "6toclip",       // receive text and binary // sfk1982: not just text
   "2tolog",        // receive text
   "2tonetlog",     // receive text
   "0beep",         // pass-thru
   "0loop",         // restart chain
   "2hextobin",     // receive text
   "8echo",         // receive nothing, force flush. sfk181
   "6hexdump",      // receive text and binary
   "1fhexdump",     // receive filenames
   "1hexfile",      // receive filenames
   "1dup",          // receive filenames
   "1copy",         // receive filenames
   "1move",         // sfk1914 internal
   "2script",       // receive TEXT
   "1fscript",      // receive TEXT
   "1ftee",         // receive filenames
   "2tee",          // receive TEXT
   "2toterm",       // receive TEXT
   "2tofile",       // receive TEXT
   "2append",       // receive TEXT
   "2appendto",     // receive TEXT
   "0sfk",          // flushes chain
   "0then",         // flushes chain
   "8status",       // receive nothing, force flush
   "1stat",         // receive filenames
   "8fromclip",     // receive nothing, force flush
   "0pause",        // receive nothing
   "0prompt",       // sfk189
   "2dec",          // receive TEXT
   "2hex",          // receive TEXT
   "2sort",         // receive TEXT
   "2count",        // receive TEXT
   "1ftp",          // receive filenames
   "1sft",          // receive filenames
   "0mkdir", "0cd", "0getcwd", "0cwd", // pass-thru
   "8ver",          // receive nothing, force flush
   "8winver",       // receive nothing, force flush
   "1md5",          // receive files
   "1crc",          // receive files
   "2wget",         // receive files and text
   "2strlen",       // receive TEXT
   "2linelen",      // receive TEXT
   "2webreq",       // receive TEXT
   "01web",         // receive TEXT, use option chainweb
   "2tweb",         // receive TEXT sfk187
   "2mweb",         // receive TEXT sfk187 internal
   "0cweb",         // receive nothing sfk1872
   "8call",         // receive nothing, force flush sfk181
   "6tcall",        // receive text and binary  sfk181
   "1fcall",        // receive filenames        sfk181
   "0if",           // receive nothing
   "6tif",          // receive TEXT and binary
   "2require",      // receive TEXT
   "2tdifflines",   // receive TEXT sfk193
   "1media",        // receive filenames
   "1filetime",     // receive filenames
   "1touch",        // receive filenames
   "1xfind",        // receive filenames sfk1953
   "6xmlform",      // receive TEXT and binary
   "6xf",           // receive TEXT and binary
   "2storetext",    // receive TEXT
   "8gettext",      // receive nothing, force flush
   "6xed",          // receive TEXT and binary
   "6xex",          // receive TEXT and binary
   "2csvtotab",     // receive TEXT
   "2csvtab",       // receive TEXT
   "2tabtocsv",     // receive TEXT
   "2tabcsv",       // receive TEXT
   "2calc",         // receive TEXT
   "6setvar",       // receive TEXT and binary
   "6addtovar",     // receive TEXT and binary
   "8getvar",       // receive nothing, force flush
   "0dropvar",      // receive nothing
   "2perline",      // receive TEXT
   "2tohtml",       // receive TEXT
   "2tovoid",       // receive TEXT
   "0end",          // pass-thru
   "6tend",         // receive text and binary  sfk181
   "1fend",         // receive filenames        sfk181
   "0stop",         // receive nothing sfk181
   "0goto",         // sfk1812
   "6tgoto",        // sfk193  text and binary
   "8tell",         // receive nothing, force flush
   "6encode",       // receive text and binary
   "6decode",       // receive text and binary
   "6udpsend",      // sfk1833 text and binary
   "0udp",          // sfk1872
   "0cudp",         // sfk1872
   "2spell",        // sfk1833 text
   "6setbytes",     // sfk1840
   "6chars",        // sfk1840
   "0for",          // receive nothing
   "0endfor",       // receive nothing
   "0rand",         // sfk189
   "2addhead",      // sfk187
   "2addtail",      // sfk187
   #ifdef _WIN32
   "6atow",  "6ansitoucs",  // sfk190
   "6iwtoa", "6iucstoansi", // sfk190
   #endif
   "6utow",  "6utftoucs",   // sfk190
   "6iwtou", "6iucstoutf",  // sfk190
   "6atou",  "6ansitoutf",  // sfk1902
   "6utoa",  "6utftoansi",  // sfk1902
   "1uuenc",        // sfk198 fnames
   "2uudec",        // sfk198 text
   "1xxenc",        // sfk198 fnames
   "2xxdec",        // sfk198 text
   #ifdef SFKPACK
   "1zipto",
   "8unzip",
   #endif // SFKPACK
   // sfk1833:
   "8fromnet", "8color", "8make-random-file",
   "8time", "8data", "8home", "8ruler",
   "8space", "8filesys",
   "6swap",
   // not yet:
   // "8ping", "8fuzz",
   // "8split", "8join",
   // "8index", "8gindex",
   // "8reflist", "8deplist",
   // "8ip", "8bin-to-src",
   "2getcol",
   "2tabcol",
   "1put",      // sfk1943 includes putall
   "1mput",     // sfk1944 convenience
   "6jsonform", // sfk1952
   "6jform",    // sfk1952
   "2cscaninc", // sfk1982 internal
   "1fjoin",    // sfk199
   "1fwav",     // sfk200
   "2passto",   // sfk200
   "1iview",    // sfk200
   "1fplay",    // sfk200
   0
};

// when passing stuff from one command to another,
// what is most probably used: text or filenames?
cchar *aGlblDefChnModes[] =
{// from       to          use default mode (1==files, 2==text)
   "sel",      "detab",    "1",
   "list",     "detab",    "1",
   "fromclip", "dv",       "2",
   0, 0, 0
};

enum eChainCodes {
   ccftt    = -1,
   ccfile   = -2,
   ccttf    = -3,
   cctext   = -4,
   ccthen   = -5,
   ccto     = -6, // text and binary
};

int checkNoPass(char *argv[], int argc, int iDir,
   int &rtype, int &rbinary, bool bForcePlus)
{
   char *pcmd=argv[iDir];

   if (*pcmd=='+')
      pcmd++;
   else
   if (bForcePlus)
      return 0;

   /*
      unsolved: cmd #(var): future content of var
      is not known, could be option, filename etc.
   */

   // sfk193 ... +setvar a=1 nopass detection
   if (!strcmp(pcmd, "setvar")) {
      char *pparm=str("");
      for (int i=iDir+1; i<argc; i++) {
         char *psz=argv[i];
         if (psz[0]=='-') continue;
         if (psz[0]=='+') break; // sfk1973 fix
         pparm=psz; break;
      }
      if (strchr(pparm,'=')) {
         if (cs.verbose)
            printf("[%s %s expects no chain data.]\n",pcmd,pparm);
         rtype = 0; // none
         return ccthen;
      }
      if (cs.verbose)
         printf("[%s %s requires chain data.]\n",pcmd,pparm);
   }

   // sfk1972 +calc nopass detection
   if (!strcmp(pcmd, "calc")) {
      char *pparm=str("");
      for (int i=iDir+1; i<argc; i++) {
         char *psz=argv[i];
         if (!strcmp(psz,"-sum")) // sfk198
            {pparm=str("#text");break;}
         if (psz[0]=='-') continue;
         if (psz[0]=='+') break;  // sfk1973 fix
         pparm=psz; break;
      }
      if (strstr(pparm,"$text") == 0
          && strstr(pparm,"#text") == 0) {
         if (cs.verbose)
            printf("[%s %s expects no chain data.]\n",pcmd,pparm);
         rtype = 0; // none
         return ccthen;
      }
      if (cs.verbose)
         printf("[%s %s requires chain data.]\n",pcmd,pparm);
   }

   return 0;
}

// rc:  0 if NO chain command
//     <0 if ++ftt, ++ttf etc.
//     >0 if one of aGlblChainCmds
int getChainCodeNoPass(char *argv[], int argc, int iDir, int &rtype, int &rbinary)
{
   char *pszin = argv[iDir];

   if (pszin[0] != '+')
      return 0;

   if (pszin[1] != '+')
      return 0;   // e.g. +then

   if (   !strcmp(pszin, "++ftt")
       || !strcmp(pszin, "++filenamestotext")
      )
   {
      rtype = 1; // files
      return ccftt;
   }

   if (   !strcmp(pszin, "++ttf")
       || !strcmp(pszin, "++texttofilenames")
      )
   {
      rtype = 2; // text
      return ccttf;
   }

   // ++list etc.

   pszin += 2;
   for (int i=0; aGlblChainCmds[i]; i++)
   {
      cchar *psz = aGlblChainCmds[i];
      int aflags = *psz - '0';
      int bbinary = (aflags & 4) ? 1 : 0;
          aflags &= (3+8);
      psz++;
      int aflags2 = 0;
      if (isdigit(*psz)) {
          aflags2 = *psz - '0';
          psz++;
      }
      if (aflags2 == 1)
         aflags = gs.cweb ? 2 : 8;
      if (!strncmp(psz, pszin, strlen(psz)))
      {
         rtype   = aflags;
         rbinary = bbinary;
         // printf("gcc: %d %d %s\n",aflags,bbinary,psz);
         return i;
      }
   }

   rtype = 0;
   return 0;
}

// rc:  0 if NO chain command
//     <0 if +text, +file etc.
//     >0 if one of aGlblChainCmds
int getChainCode(char *argv[], int argc, int iDir, int &rtype, int &rbinary)
{
   if (gs.nopass)
      return getChainCodeNoPass(argv, argc, iDir, rtype, rbinary);

   char *pszin=argv[iDir];

   if (   !strcmp(pszin, "+ftt")
       || !strcmp(pszin, "+filenamestotext")
      )
   {
      rtype = 1; // files
      return ccftt; // == -1
   }

   // if (!strcmp(pszin, "+files")) // del sfk1840
   // {
   //    rtype = 1; // files
   //    return ccfile;
   // }

   if (   !strcmp(pszin, "+ttf")
       || !strcmp(pszin, "+texttofilenames")
      )
   {
      rtype = 2; // text
      return ccttf;
   }

   // if (!strcmp(pszin, "+text")) // del sfk1840
   // {
   //    rtype = 2; // text
   //    return cctext;
   // }

   if (!strcmp(pszin, "+to"))
   {
      rtype = 6; // text+binary
      return ccto;
   }

   if (!strcmp(pszin, "+then"))
   {
      rtype = 0; // none
      return ccthen;
   }

   // --- sfk193: ... +setvar a=foo nopass detection [218991] ---
   if (checkNoPass(argv,argc,iDir,rtype,rbinary,1) == ccthen) // nopass.setvar
      return ccthen;
   // --- ---

   if (pszin[0] == '+')
   {
      pszin++;
      for (int i=0; aGlblChainCmds[i]; i++)
      {
         cchar *psz = aGlblChainCmds[i];
         int aflags = *psz - '0';
         int bbinary = (aflags & 4) ? 1 : 0;
             aflags &= (3+8);
         psz++;
         int aflags2 = 0;
         if (isdigit(*psz)) {
             aflags2 = *psz - '0';
             psz++;
         }
         // sfk187: +web chain input default option
         if (aflags2 == 1)
            aflags = gs.cweb ? 2 : 8;
         if (!strncmp(psz, pszin, strlen(psz)))
         {
            rtype   = aflags;
            rbinary = bbinary;
            // printf("gcc: %d %d %s\n",aflags,bbinary,psz);
            return i;
         }
      }
   }

   rtype = 0;
   return 0;
}

// find next chain command clearly defining a type like FILES or TEXT
// rc: 1==files, 2==text, 0==none
// rforce: type is defined by +text etc. and MUST be used,
//         otherwise caller may use defaults
int findNextChainType(int iDir, char *argv[], int argc, char **pszNext, bool &rforce, int &rbinary)
{
   if (cs.nochain) return 0;

   for (; iDir < argc; iDir++)
   {
      char *psz = argv[iDir];

      if (!psz)
      {
         // happens on %n parm of a script which is missing
         perr("missing or wrong script parameter (%d/%d)", iDir, argc);
         for (int i=0; i<argc; i++)
            printf("  #%d %s\n",i,argv[i]?argv[i]:"<null>");
         return 0;
      }

      int ntype = 0, bbinary = 0;
      int ncode = getChainCode(argv, argc, iDir, ntype, bbinary);

      // fix: 1763: everything after +then must be ignored.
      if (strcmp(psz, "+then") == 0 || ntype != 0)
      {
         if (psz[0] == '+') psz++;
         *pszNext = psz;
         // is it a forced type like +text?
         if (ncode < 0)
            rforce = 1;
         rbinary = bbinary;
         return ntype;
      }

      // +label name is always transparent
      if (!strcmp(psz, "+label")) { // sfk193 nopass.generic exception
         iDir++; // skip label name
         continue;
      }

      if (!cs.keepdata) // sfk193 nopass.generic: disable data tunneling
         break;
   }

   return 0;
}

bool isChainStartInt(char *pszCmd, char *argv[], int argc, int iDir, int *iDirNext, bool bAllowVerbose=0)
{
   if (cs.nochain) return 0;

   // prefix mode switchers:
   int nplus1=0, nplus2=0;
   int ntype = 0, bbin1 = 0;
   int ncode = getChainCode(argv, argc, iDir, ntype, bbin1);

   if (gs.nopass) // sfk1972 internal
   {
      char *psz = argv[iDir];
      if (psz[0] != '+')
         return 0;
      // is +cmd or ++cmd
      if (iDirNext) *iDirNext = iDir;
      if (psz[1] != '+') {
         return 1;
      }
      // is ++cmd
      if (ntype & 1) chain.colfiles  = 1;
      if (ntype & 2) chain.coldata   = 1;
      if (bbin1)     chain.colbinary = 1;
      if (cs.tracechain)
         printf("[chain from %s to %s. collect f=%d t=%d b=%d, n=%u idir %d %p]\n",pszCmd,psz,chain.colfiles,chain.coldata,chain.colbinary,ntype,iDir,iDirNext);
      return 1;
   }

   switch (ncode)
   {
      case ccftt:
         chain.colfiles   = 1;
         chain.files2text = 1;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         iDir++;
         nplus1 = 1;
         break;

      case ccfile:
         chain.colfiles = 1;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         iDir++;
         nplus1 = 1;
         break;

      case ccttf:
         chain.coldata    = 1;
         chain.text2files = 1;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         iDir++;
         nplus1 = 1;
         break;

      case cctext:
         chain.coldata = 1;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         iDir++;
         nplus1 = 1;
         break;

      case ccto:
         chain.coldata = 1;
         chain.colbinary = 1;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         iDir++;
         nplus1 = 1;
         break;

      case ccthen:
         chain.coldata    = 0;
         chain.colfiles   = 0;
         if (iDir >= argc-1)
            { perr("missing +command after %s\n", argv[iDir]); exit(9); }
         // use "then" as next chain command
         break;

      default:
         break;   // fall trough
   }
 
   // if +file or +text is specified, following command
   // is not required to start with "+".

   char *pszParm = argv[iDir];
   if (!strncmp(pszParm, "+", 1)) {
      nplus2 = 1;
      pszParm++;
   }

   if (ncode == ccthen) {
      // +then stops any further chain detections:
      if (iDirNext) *iDirNext = iDir;
      if (cs.tracechain)
         printf("[chain from %s to %s. collect f=%d t=%d b=%d, idir %d %p]\n",pszCmd,pszParm,chain.colfiles,chain.coldata,chain.colbinary,iDir,iDirNext);
      return true;
   }
 
   bool bForceFlush = 0;

   if (nplus1 || nplus2)
   {
      if (nplus2)
         nplus1 = 0;
      for (int i=0; aGlblChainCmds[i]; i++)
      {
         cchar *psz = aGlblChainCmds[i];
         int aflags = *psz - '0';
         int bbin2  = (aflags & 4) ? 1 : 0;
             aflags &= (3+8);
         psz++;
         int aflags2 = 0;
         if (isdigit(*psz)) {
             aflags2 = *psz - '0';
             psz++;
         }
         // sfk187: +web chain input default option
         if (aflags2 == 1)
            aflags = gs.cweb ? 2 : 8;
         if (!strncmp(psz, pszParm, strlen(psz)))
         {
            // standing on the first following chain command.
            if (!chain.coldata && !chain.colfiles)
            {
               // find out what the next typed chain element is,
               // e.g. +noop +sleep 1000 +text +filter -+foo
               // requires to find +text.
               bool bForce    = 0;
               char *pszNext  = 0;
               int  bbin3     = 0;
               int nNextType = findNextChainType(iDir, argv, argc, &pszNext, bForce, bbin3);
               mtklog(("chain: ics: found chain type %d at %s\n", nNextType, pszNext));
 
               if (cs.tracechain && pszNext)
                  printf("[found chain type %d (bin=%d) at %s]\n", nNextType, bbin3, pszNext);

               // if next type is not a forced one like +text,
               if (!bForce && pszNext)
               {
                  // check if there is a default mapping
                  for (int k=0; aGlblDefChnModes[k]; k += 3)
                  {
                     // look into defaults table
                     cchar *pszFrom = aGlblDefChnModes[k+0];
                     cchar *pszTo   = aGlblDefChnModes[k+1];
                     cchar *pszMode = aGlblDefChnModes[k+2];
                     if (   !strncmp(pszCmd , pszFrom, strlen(pszFrom))
                         && !strncmp(pszNext, pszTo  , strlen(pszTo  ))
                        )
                     {
                        // found a default:
                        nNextType = pszMode[0] - '0';
                        if (cs.tracechain)
                           printf("[but using default type mapping %d]\n", nNextType);
                        break;
                     }
                  }
               }
 
               if (nNextType & 1) {
                  chain.colfiles = 1;
                  chain.colbinary = bbin3;
               }

               if (nNextType & 2) {
                  chain.coldata = 1;
                  chain.colbinary = bbin3;
               }

               if (nNextType & 8) { // sfk181
                  chain.coldata = 0;
                  chain.colbinary = 0;
                  bForceFlush = 1;
               }
            }
 
            // if no mode yet set, use command default, if any
            if (!bForceFlush && !chain.coldata && !chain.colfiles)
            {
               if (aflags==1)
                  chain.colfiles = 1;
               else
               if (aflags==2)
                  chain.coldata  = 1;
               chain.colbinary = bbin2;
            }
 
            if (iDirNext) *iDirNext = iDir;

            if (cs.tracechain)
               printf("[chain from %s to %s. collect f=%d t=%d b=%d, idir %d %p]\n",pszCmd,pszParm,chain.colfiles,chain.coldata,chain.colbinary,iDir,iDirNext);
 
            return true;
         }
      }
   }
 
   if (nplus2) {
      // no chain start although "+" parm
      // find/grep have a very loose syntax, therefore
      if (!strcmp(pszCmd, "find") || !strcmp(pszCmd, "grep"))
         return false; // command handler will issue error
      // check also filter parms
      if (!strncmp(argv[iDir], "++", 2) || !strncmp(argv[iDir], "+ls", 3)) // deprecated with sk197
         return false; // accept non-chain start
      // sfk1892: consider +anycommand as chaining without data
      chain.coldata    = 0;
      chain.colfiles   = 0;
      if (iDirNext) *iDirNext = iDir;
      return true;
      /*
      char *psz = argv[iDir];
      perr("command does not support input chaining: %s\n", psz);
      if (*psz=='+') psz++;
      pinf("try \"+then %s\" if \"%s\" requires no input data\n", psz, psz);
      exit(9);
      */
   }
 
   return false;
}

bool isChainStart(char *pszCmd, char *argv[], int argc, int iDir, int *iDirNext, bool bAllowVerbose=0)
{
   bool bres = isChainStartInt(pszCmd, argv, argc, iDir, iDirNext, bAllowVerbose);

   if (   (bres!=0 || iDir+1>=argc)
       && cs.iotrace && szIOTraceBuf[0] // isChainStart
      )
   {
      bool bcoldata   = chain.coldata;
      bool bcolbinary = chain.colbinary;
      bool bcolfiles  = chain.colfiles;
      if (!strcmp(pszCmd, "stop")) {
         bcoldata = bcolbinary = bcolfiles = 0;
      }
      char szOutInfo[200];
      char *pszTarget = argv[iDir];
      if (bcoldata || bcolbinary) {
         snprintf(szOutInfo,sizeof(szOutInfo)-10, "=> send to %s", pszTarget);
      } else if (bcolfiles) {
         snprintf(szOutInfo,sizeof(szOutInfo)-10, "## %s may use files", pszTarget);
      } else {
         if (!strcmp(pszCmd, "tend") || !strcmp(pszCmd, "fend"))
            strcpy(szOutInfo, "## return chain  ");
         else
            strcpy(szOutInfo, "##               "); // ## print toterm
      }
      fprintf(stderr, "%s %s\n", szIOTraceBuf, szOutInfo);
      szIOTraceBuf[0] = '\0';
   }

   return bres;
}

int addConveniencePathMask(char *psz1, int &lRC, int iFromInfo)
{
   // turn a file mask into a path mask
   static char szMaskBuf[100];

   if (cs.debug)
      printf("] addConvPathMask for %s (from=%d)\n", psz1, iFromInfo);

   if (containsWildCards(psz1))
      strcopy(szMaskBuf, psz1);
   else
   if (isNotChar(*psz1))
      snprintf(szMaskBuf, sizeof(szMaskBuf)-10, "%c*%s", glblNotChar, psz1+1);
   else
      snprintf(szMaskBuf, sizeof(szMaskBuf)-10, "*%s", psz1);
   if (cs.verbose > 1)
      pinf("auto-converting %s to path mask %s\n", psz1, szMaskBuf);
   if (lRC |= glblFileSet.addDirMask(szMaskBuf)) // is copied
      return 9;
   return 0;
}

/*
   this implements highly detailed file selection, like
   -dir mydir -subdir sub1 !tmp -file .txt !old -dir mydir2 ...
*/
int processDirParms(char *pszCmd, int argc, char *argv[], int iDir, int nModeFlags, int *iDirNext, bool *pAnyDone)
{__
   mtklog(("processDirParms with argc=%d",argc));

   if (cs.debug) printf("process dir parms\n");

   int  nAutoComplete = (nModeFlags & 3);

   bool bStrictOptions = (nModeFlags & 4) ? 1 : 0;
   // strict: accept only -dir ... -file ... but not -pat and general opts,
   //         except for high prio options: -yes.
   //         used by: replace, xreplace, rename, xrename

   bool bReturnOptions = (nModeFlags & 8) ? 1 : 0; // sfk197 expclicitely
   // unknown options should not produce an error,
   // but return to caller for further processing.

   bool bAnyDone = 0; // any user-supplied dir/file parm used
   bool bPreFileFlank = 0;
   bool bConvenience = 0;
   int lRC = 0;

   bool aStateTouched[eST_MAXStates+10];
   memset(aStateTouched, 0, sizeof(aStateTouched));

   // fetch prefixed general options
   while (iDir < argc) {
      if (setGeneralOption(argv, argc, iDir))
         iDir++;
      else if (strlen(argv[iDir])==0)  // sfk1813 empty parm support
         iDir++;
      else
         break;
   }

   if (iDir < argc)
   {
      // check for -root a -using b now as it requires to rebuild text
      bool bRootWithUsing = 0;
      bool bUseUsingSyntax = 0;  // no '#' remarks. use '//' remarks.

      if (!strcmp(argv[iDir], "-root")) do {
         if (iDir+1 >= argc) return 9+perr("missing parameter after -root");
         if (iDir+2 >= argc) break; // no -root x -using y
         if (strcmp(argv[iDir+2], "-using")) break; // no -root x -using y
         bRootWithUsing = 1;
      } while (0);

      // do we have to rebuild a script text?
      if (!strcmp(argv[iDir], "-fileset")
          || !strcmp(argv[iDir], "-using")   // sfk197
          || bRootWithUsing                  // sfk197 -root a -using b
          )
      {
         // re-create parameter array from input file or label text

         char *pszParmFile = 0;
         int   iParmBytes  = 0;

         if (!strcmp(argv[iDir], "-using") || bRootWithUsing)
         {
            // sfk1963: -using label1 [-using label2] [...]
            // sfk197 : -root C: -using label1 -root D: -using label2
            while (iDir < argc)
            {
               // this is ONLY for -root a -using b -root c -using d.
               // this is NOT  for -root a -dir mydir.
               if (!strcmp(argv[iDir], "-root"))
               {
                  if (iDir+1 >= argc) return 9+perr("missing parameter after -root");
                  if (iDir+2 >= argc) break; // no -root x -using y
                  if (strcmp(argv[iDir+2], "-using")) break; // no -root x -using y

                  iDir++;

                  snprintf(szLineBuf, MAX_LINE_LEN, "-root \"%s\" ", argv[iDir]);
                  int ilen = strlen(szLineBuf);

                  if (pszParmFile == 0) {
                     pszParmFile = new char[ilen+10];
                     memcpy(pszParmFile, szLineBuf, ilen);
                     pszParmFile[ilen] = '\0';
                     iParmBytes = ilen;
                     if (cs.debug) printf("[-root %s with %d chars]\n", argv[iDir], ilen);
                  } else {
                     char *ptmp = new char[iParmBytes+ilen+10];
                     memcpy(ptmp, pszParmFile, iParmBytes);
                     memcpy(ptmp+iParmBytes, szLineBuf, ilen);
                     ptmp[iParmBytes+ilen] = '\0';
                     iParmBytes += ilen;
                     delete [] pszParmFile;
                     pszParmFile = ptmp;
                     if (cs.debug) printf("[-root %s with %d chars]\n", argv[iDir], ilen);
                  }

                  iDir++;
                  continue;
               }

               if (strcmp(argv[iDir], "-using"))
                  break;

               bUseUsingSyntax = 1;

               iDir++;
               if (iDir >= argc) return 9+perr("missing label name after -using");
               char *plabel  = argv[iDir++];
               char *pscript = pGlblCurrentScript;
               if (!pscript) return 9+perr("did not get own script text");

               char szpat[100];
               snprintf(szpat, sizeof(szpat)-10, "sfk label %s", plabel);
               char *plabtext = strstr(pscript, szpat);
               if (!plabtext) return 9+perr("did not find label: %s", plabel);
               plabtext += strlen(szpat);
               skipWhite(&plabtext);

               char *peot = strstr(plabtext, "+end\r");
               if (!peot) peot = strstr(plabtext, "+end\n");
               if (!peot)
                  return 9+perr("did not find +end line after label: %s", plabel);

               int ilen = peot - plabtext;

               if (pszParmFile == 0) {
                  // first -using label
                  pszParmFile = new char[ilen+10];
                  memcpy(pszParmFile, plabtext, ilen);
                  pszParmFile[ilen] = '\0';
                  iParmBytes = ilen;
                  if (cs.debug) printf("[-using %s with %d chars]\n", plabel, ilen);
               } else {
                  // -using label1 -using label2
                  char *ptmp = new char[iParmBytes+ilen+10];
                  memcpy(ptmp, pszParmFile, iParmBytes);
                  memcpy(ptmp+iParmBytes, plabtext, ilen);
                  ptmp[iParmBytes+ilen] = '\0';
                  iParmBytes += ilen;
                  delete [] pszParmFile;
                  pszParmFile = ptmp;
                  if (cs.debug) printf("[-using %s with %d chars (%d total)]\n", plabel, ilen, iParmBytes);
               }
            }
         }
         else
         {
            // -fileset filename
            iDir++;
            if (iDir >= argc) return 9+perr("missing filename name after -fileset");
            char *pszFilename = argv[iDir++];
            pszParmFile = loadFile(pszFilename);
            if (!pszParmFile) return 9;
         }

         pGlblFileParms = new StringTable(); // processDirParms

         // tokenize -fileset text
         char *psz1 = pszParmFile;
         while (*psz1)
         {
            // find next token.
            skipSpaceRem(&psz1, bUseUsingSyntax); // pdp
            if (!*psz1) break; // fix sfk1963

            // find end of token. support "parm with blanks".
            char *psz2 = psz1+1;
            if (*psz1 == '"')
               while (*psz2 && *psz2!='"')
                  psz2++;
            while (*psz2 && *psz2!=' ' && *psz2!='\t' && *psz2!='\r' && *psz2!='\n')
               psz2++;

            // isolate token
            if (*psz2)
               *psz2++ = '\0';

            // line-end may lead to empty entry, therefore
            if (strlen(psz1)) // only if not line-end
            {
               // strip "", if any
               if (*psz1 == '"' && strlen(psz1) >= 2) {
                  psz1++;
                  int nLen = strlen(psz1);
                  if (psz1[nLen-1] == '"')
                      psz1[nLen-1] = '\0';
               }
               if (cs.debug) printf("token: <<%s>>\n", psz1);
               pGlblFileParms->addEntry(psz1);
            }

            // continue with next token, if any
            psz1 = psz2;
         }
         delete [] pszParmFile;

         // pGlblFileParms now holds all parms
         int nParms = pGlblFileParms->numberOfEntries();
         apGlblFileParms = new char*[nParms];
         for (int i=0; i<nParms; i++)
            apGlblFileParms[i] = pGlblFileParms->getEntry(i, __LINE__);
         nGlblFileParms = nParms;

         // fix sfk1953: -fileset stopped command chain.
         if (iDirNext) {
            int iNext = iDir; // sfk1963 not +1 see above
            int iChainNext = 0;
            for (; iNext < argc; iNext++) {
               if (isChainStart(pszCmd, argv, argc, iNext, &iChainNext))
                  { *iDirNext = iChainNext; break; }
               if (!strcmp(argv[iNext], "-using")) { // sfk1963
                  char *pszInfo = str("");
                  if (iNext+1 < argc) pszInfo = argv[iNext+1];
                  return 9+perr("option not allowed here: %s %s (1)", argv[iNext], pszInfo);
               }
            }
         }

         // restart through recursion
         // printf("[%d parms from file]\n", nParms);
         return processDirParms(pszCmd, nParms, apGlblFileParms, 0, nAutoComplete, 0, pAnyDone); // sfk1963: missing pAnyDone
      }
 
      // init fileset
      if (glblFileSet.beginLayer(false, __LINE__))
         return 9;

      // fetch further prefix options, if any
      char *pszFirstParm = argv[iDir];
      while (!strncmp(pszFirstParm, "-", 1)) {
         if (!setGeneralOption(argv, argc, iDir))
            break; // unknown option, fall through to further processing
         pszFirstParm = (iDir < argc-1) ? argv[++iDir] : (char*)"";
      }

      if (*pszFirstParm == '-')
      {
         if (cs.debug) printf("process long format\n");

         // process int format, allowing multiple dirs:
         // sfk cmd -dir dir1 dir2 !dir3 !dir4 -file .hpp .cpp

         // we now have:
         //   clRootDirs , two rows, DirName(empty) and Cmd(empty)
         // one layer:
         //   clDirMasks , one row , empty
         //   clFileMasks, one row , empty

         int lState = eST_Idle;
         for (; iDir < argc; iDir++)
         {
            // prefix -dir -file combi end marker
            if (!strcmp(argv[iDir], "-enddir")) {
               // continue locally with command parms
               if (iDir+1 >= argc)
                  return 9+perr("-enddir is not allowed as last option");
               if (iDirNext)
                  *iDirNext = iDir+1;
               // fall through to autocomplete
               break;
            }

            if (isChainStart(pszCmd, argv, argc, iDir, iDirNext, 1))
               break;

            // on every state switch, we're first landing here
            if (lState >= 0 && lState < (int)(sizeof(aStateTouched)/sizeof(bool)))
               aStateTouched[lState] = 1;

            char *psz1 = argv[iDir];
 
            if (cs.debug) printf("] \"%s\" [\n",psz1);

            if (!strcmp(psz1, "-debug")) {
               cs.debug = 1;
               continue;
            }

            if (!strcmp(psz1, "-root")) { // sfk197
               iDir++;
               if (iDir >= argc)
                  return 9+perr("missing parameter after -root");
               strcopy(cs.rootdir, argv[iDir]);
               continue;
            }

            if (!strcmp(psz1, "-using")) { // sfk197 this is done above
               char *pszInfo = str("");
               if (iDir+1 < argc) pszInfo = argv[iDir+1];
               return 9+perr("option not allowed here: %s %s (2)", psz1, pszInfo);
            }

            if (!strcmp(psz1, "-dir"))
            {
               aStateTouched[eST_RootDirs] = 1;
               // creation of new dir/file mask layers:
               // 1. PostFileFlank: on change from non -dir to -dir
               // 2. PreFileFlank: on change from -file to -dir
               if (lState == eST_FileMasks && !bPreFileFlank) {
                  if (cs.debug) printf("] -dir: begin layer\n");
                  // not the very first -dir parm: add another layer
                  if (glblFileSet.beginLayer(false, __LINE__))
                     return 9;
               }
               else
               if (lState == eST_SubDirs) {
                  if (cs.debug) printf("] -dir: begin layer.3\n");
                  // not the very first -dir parm: add another layer
                  if (glblFileSet.beginLayer(false, __LINE__))
                     return 9;
               }
               else
               if (lState == eST_RootDirs && !bPreFileFlank) {
                  // -dir tmp1 tmp2 -dir tmp3 -file .txt
                  // add implicite "-file *" to first layer
                  lState = eST_FileMasks;
                  lRC |= glblFileSet.addFileMask(str("*"));
                  aStateTouched[lState] = 1;
                  if (cs.debug) printf("] -dir: begin layer.2\n");
                  if (glblFileSet.beginLayer(false, __LINE__))
                     return 9;
               }
               else {
                  if (cs.debug) printf("] -dir: no begin layer\n");
               }
               // will collect further root dirs next
               lState = eST_RootDirs;
               aStateTouched[eST_FileMasks] = 0;
               continue;
            }
 
            if (!strcmp(psz1, "-sub") || !strcmp(psz1, "-subdir"))
            {
               aStateTouched[eST_SubDirs] = 1;
               if (lState != eST_RootDirs) {
                  perr("-sub[dir] is allowed only after -dir");
                  return 9;
               }
               lState = eST_SubDirs;
               continue;
            }

            if (!strcmp(psz1, "-any")) {
               // list of file- and directory names follows.
               cs.anyused = 1;
               lState = eST_DirFile;
               continue;
            }

            if (   isNotChar(psz1[0]) != 0
                && lState != eST_SubDirs
               )
            {
               if (lState == eST_RootDirs)
                  lRC |= glblFileSet.addDirMask(psz1);
               else
                  lRC |= glblFileSet.addFileMask(psz1);
               bAnyDone = 1;
               continue;
            }

            // collection of directory / file path masks:
            if (lState == eST_RootDirs)
            {
               if (psz1[0] == '+') {
                  if (!glblFileSet.rootDirs().numberOfEntries()) {
                     // +dirmask supplied without any previous root dir.
                     // imply that user wants dirs within the current dir "."
                     glblFileSet.addRootDir(str("."), __LINE__, false);
                     // imply that user does NOT want to process files from "."
                  }
                  bGlblNoRootDirFiles = 1;
                  lRC |= glblFileSet.addDirMask(psz1);
                  bGlblHavePlusDirMasks = 1;
                  bAnyDone = 1;
                  continue;
               }
               else
               if (containsWildCards(psz1))
               {
                  if (!glblFileSet.rootDirs().numberOfEntries())
                     glblFileSet.addRootDir(str("."), __LINE__, false);
                  lRC |= glblFileSet.addDirMask(psz1);
                  bAnyDone = 1;
                  continue;
               }
            }

            if (!strcmp(psz1, "-file"))
            {
               aStateTouched[eST_FileMasks] = 1;
               if (!strncmp(pszCmd, "freezeto=", strlen("freezeto=")))
                  return 9+perr("no -file masks supported with freezeto command.\n");
               if (cs.shortsyntax)
                  return 9+perr("you specified a dir name in short syntax. -file is not allowed then.\n");
               // FIX sfk196: -dir ... -subdir ... -dir selected too few.
               // check for eST_SubDirs was missing, causing beginLayer skip.
               if (lState != eST_RootDirs && lState != eST_SubDirs)
                  bPreFileFlank = 1; // -file coming before -dir
               else
               if (bPreFileFlank) {
                  // not the very first -file parm: add another layer
                  if (glblFileSet.beginLayer(false, __LINE__))
                     return 9;
               }
               lState = eST_FileMasks;
               continue;
            }

            // used only with grep:
            if (!bStrictOptions && !strcmp(psz1, "-pat")) { // pdp
               lState = eST_GrepPat;
               continue;
            }

            // general option specified inbetween:
            if (bStrictOptions) {
               // unmaskable high prio general options only:
               // FIX 1652: always handle -yes even with strict -dir ... parsing.
               if (!strcmp(psz1, "-yes"))    { cs.yes = 1; continue; }
               // FIX 1660: allow -justrc being written rightmost.
               if (!strcmp(psz1, "-justrc")) { cs.justrc = 1; continue; }
            } else {
               if (setGeneralOption(argv, argc, iDir))
                  continue;
            }

            // workaround for "*" under unix. see also short syntax.
            if (!strcmp(psz1, "-all")) {
               if (lState != eST_FileMasks)
                  return 9+perr("wrong context for -all. use within -file.\n");
               // else fall through to add.
            }
            else
            if (*psz1 == '-') {
               // CHG: 1.69: continue locally with command parms.
               // used by: filter, (x)replace, (x)rename
               // sfk197: only with bReturnOptions set by those commands.
               if (bReturnOptions && iDirNext) {
                  *iDirNext = iDir;
                  // fall through to autocomplete
                  break;
               }
               perr("unknown dir or file parameter: %s\n", psz1);
               pinf("try to specify %s before -dir ... -file\n", psz1);
               return 9;
            }

            // handle all non-command parameters
            switch (lState)
            {
               case eST_RootDirs : {
                  // add another root dir, referencing the current layer.
                  if (glblFileSet.addRootDir(psz1, __LINE__, true, true)) // sfk193: -dir x.zip
                     return 9;
                  bAnyDone = 1;
                  break;
               }
               case eST_SubDirs : {
                  // add another dir mask, referencing the current layer.
                  #ifdef SFKOFFICE
                  // zip.check.mask office.long.subdir
                  // office: extend -subdir .xlsx to -subdir *.xlsx
                  if (cs.office && endsWithOfficeExt(psz1,150)) {
                     if (addConveniencePathMask(psz1,lRC,1))
                        return 9;
                  } else
                  #endif // SFKOFFICE
                  if (glblFileSet.addDirMask(psz1))
                     return 9;
                  bAnyDone = 1;
                  break;
               }
               case eST_FileMasks:
                  #ifdef SFKOFFICE
                  // zip.check.mask office.long.file
                  // office: extend -file .xlsx to -subdir *.xlsx
                  if (cs.office && endsWithOfficeExt(psz1,150)) {
                     if (addConveniencePathMask(psz1,lRC,2))
                        return 9;
                     bConvenience = 1;
                  } else
                  #endif // SFKOFFICE
                  lRC |= glblFileSet.addFileMask(psz1);
                  break;
               case eST_GrepPat  : {
                  if (!strncmp(psz1, "\\\\", 2) || !strncmp(psz1, "\\+", 2) || !strncmp(psz1, "\\-", 2))
                     psz1++;
                  glblGrepPat.addString(psz1);
                  break;
               }
               case eST_IncBin   : glblIncBin.addString(psz1); break;
               case eST_DirFile  : {
                  // used only in case of explorer drag+drop.
                  Coi ocoi(psz1, 0);
                  if (ocoi.isTravelDir()) {
                     // coi name may have been redirected
                     if (glblFileSet.addRootDir(ocoi.name(), __LINE__, true, false)) // sfk193 x.zip
                        return 9;
                  } else {
                     if (cs.debug) printf("add2flist.2: %s\n", psz1);
                     glblSFL.addEntry(ocoi); // is copied
                  }
                  bAnyDone = 1;
                  break;
               }
               default : {
                  // reached only with -root ... and plain words,
                  // which is an illegal mix of long and short format.
                  perr("unexpected parameter: %s\n", psz1); // sfk197 missing
                  return 9;
               }
            }  // endswitch
         }  // endfor iDir

         // FIX: 169: [2131217] missing write back
         if (iDirNext!=0 && iDir>=argc)
            *iDirNext = 0;
      }
      else
      if (isChainStart(pszFirstParm, argv, argc, iDir, iDirNext))
      {
         // no actual dir parms at all, instead "+end" etc.:
         mtklog(("pdp: no parms, first is chain: %s", pszFirstParm));
         // fall through to autocomplete, if any
      }
      else
      if (strlen(pszFirstParm))
      {
         // process short format, either with single dir and fpatterns
         //    . .cpp .hpp !.hppx
         // or with a list of specified file names
         //    test1.txt test2.txt
         // OR, if simple drag+drop from explorer, mixed list.
         mtklog(("pdp: short: first=%s havemdfl=%d", pszFirstParm,bGlblHaveMixedDirFileList));

         if (cs.debug) printf("process short format\n");

         Coi *pcoi = 0;
         #ifdef VFILEBASE
         pcoi = glblVCache.get(pszFirstParm);
         #endif // VFILEBASE
         if (!pcoi) {
            pcoi = new Coi(pszFirstParm, 0);
            pcoi->incref("pdp");
         }
         CoiAutoDelete odel(pcoi, 1); // with decref

         // sfk1944: must treat office file parm as normal file (1)
         if ((!bGlblHaveMixedDirFileList) && pcoi->isTravelDir(1))
         {
            // fetch dir. coi name may have been redirected.
            glblFileSet.addRootDir(pcoi->name(), __LINE__, false, false);
            bAnyDone = 1;
            iDir++;

            // fetch masks
            for (; iDir < argc; iDir++)
            {
               if (isChainStart(pszCmd, argv, argc, iDir, iDirNext))
                  break;

               // also care about postfix/inbetween options
               char *psz1 = argv[iDir];
               if (setGeneralOption(argv, argc, iDir))
                  continue;
               if (!strcmp(psz1, "-file"))
                  return 9+perr("mixing of short and long syntax not allowed. you may try -dir %s -file ...\n", pszFirstParm);
               if (!strcmp(psz1, "-dir"))
                  return 9+perr("mixing of short and long syntax not allowed. you may try -dir %s ...\n", pszFirstParm);

               // workaround for "*" under unix. see also int syntax.
               if (!strcmp(psz1, "-all")) {
                  // fall through to add
               }
               else
               if (psz1[0] == '-') {
                  perr("unexpected option in short filename list: %s\n", psz1);
                  pinf("try specifying %s directly after %s\n", psz1, pszCmd);
                  return 9;
               }

               #if 0 // def SFKOFFICE - no file mask conversion
               // zip.check.mask office.short.file
               // office: extend short file mask to -subdir *.xlsx
               if (cs.office && endsWithOfficeExt(psz1,151)) {
                  if (addConveniencePathMask(psz1,lRC,3))
                     return 9;
               } else
               #endif // SFKOFFICE
               #ifdef VFILEBASE
               // zip.check.mask zip.short.file
               // zip: extend short file mask to -subdir *.zip
               if (!cs.shallowzips && cs.xelike && cs.travelzips && isArcFile(psz1)) {
                  if (addConveniencePathMask(psz1,lRC,4))
                     return 9;
               } else
               #endif // VFILEBASE
               if (lRC |= glblFileSet.addFileMask(psz1))
                  return 9;
               bAnyDone = 1;
            }
         }
         else
         {
            // todo: split this in 2 paths. there can be no options
            //       in case of drag+drop on .exe.

            // fetch file and/or dir list. this path is called
            // - either from command prompt
            // - or from drag+drop on .exe (no preconfigured icon)
            for (; iDir < argc; iDir++)
            {
               if (isChainStart(pszCmd, argv, argc, iDir, iDirNext))
                  break;

               char *psz1 = argv[iDir];
               if (*psz1 == '-') {
                  if (!setGeneralOption(argv, argc, iDir))
                     return 9+perr("wrong context or unknown option: %s\n", psz1);
               } else {
                  char *pszroot = getAbsPathStart(psz1); // sfk1934 fname parms
                  Coi ocoi(psz1, pszroot); // sfk1934 root with fname parms
                  if (bGlblHaveMixedDirFileList && ocoi.isAnyDir()) {
                     glblFileSet.addRootDir(psz1, __LINE__, false, false);
                     bAnyDone = 1;
                  } else {
                     if (cs.debug) printf("add2flist.3: %s\n", psz1);
                     glblSFL.addEntry(ocoi); // is copied
                     bAnyDone = 1;
                  }
               }
            }
            if (!bGlblHaveMixedDirFileList)
               nAutoComplete = 0;
         }
      }
   }
   else
   {
      // no parms at all supplied:
      if (!cs.blockAutoComplete && (nAutoComplete & 2))
         glblFileSet.addRootDir(str("."), __LINE__, false);
      // possibly redundant, see autocomplete below
   }

   bool bFail = (lRC != 0);

   // plausibility checks
   if (aStateTouched[eST_IncBin] && !glblIncBin.numberOfEntries())
      { bFail=1; perr("please supply a list of file extensions after -addbin or -include-binary-files.\n"); }
   if (aStateTouched[eST_GrepPat] && !glblGrepPat.numberOfEntries())
      { bFail=1; perr("please supply some pattern words after option -pat.\n"); }
   if (!bConvenience && aStateTouched[eST_FileMasks] && !glblFileSet.fileMasks().numberOfEntries())
      { bFail=1; perr("please supply some file extensions after option -file.\n"); }
   if (aStateTouched[eST_RootDirs] && !glblFileSet.rootDirs().numberOfEntries())
      { bFail=1; perr("please supply some directory names after option -dir.\n"); }

   if (!cs.blockAutoComplete && (nAutoComplete != 0))
      glblFileSet.autoCompleteFileMasks(nAutoComplete);

   if (aStateTouched[1] || aStateTouched[2]) // if any -dir or -file
      if (glblFileSet.checkConsistency()) // then no empty layers allowed
         bFail=1;

   if (cs.debug) glblFileSet.dump();

   glblFileSet.setBaseLayer();

   // -sincedir requires root dir parameters
   if (pszGlblSinceDir && !glblFileSet.hasRoot(0)) {
      // Array &aroots = glblFileSet.rootDirs();
      // int nroots = aroots.numberOfEntries(0);
      // if (nroots <= 0)
      return 9+perr("-sincedir/add/diff requires two directories.\n");
   }

   // tell caller if any real dir parms were used
   if (pAnyDone) *pAnyDone = bAnyDone;

   return bFail ? 9 : 0;
}

int setProcessSingleDir(char *pszDirName)
{__
   #ifdef VFILEBASE
   if (   !strBegins(pszDirName, "http://")
       && !strBegins(pszDirName, "https://")
       && !strBegins(pszDirName, "ftp://")
      )
   #endif // VFILEBASE
   {
      Coi ocoi(pszDirName, 0);
      if (!ocoi.isTravelDir())
         return 9+perr("directory not found: %s\n", pszDirName);
   }

   // init fileset
   if (glblFileSet.beginLayer(true, __LINE__))
      return 9;

   glblFileSet.addRootDir(pszDirName, __LINE__, false, false);
   glblFileSet.autoCompleteFileMasks(3);

   if (cs.debug) glblFileSet.dump();

   glblFileSet.setBaseLayer();

   return 0;
}



void resetFileSet()
{
   glblFileSet.reset();

   // sfk1963: missing reset of -fileset etc. parms between commands
   if (pGlblFileParms)  { delete pGlblFileParms; pGlblFileParms=0; }
   if (apGlblFileParms) { delete [] apGlblFileParms; apGlblFileParms=0; }
}

void resetAllFileSets()
{
   resetFileSet();   // sfk1963 safety

   chain.usefiles = 0;
   bGlblStdInAny  = 0;
   glblSFL.resetEntries();
}

int walkAllTreesInt(int nFunc, int &rlFiles, int &rlDirs, num &rlBytes);
int checkAllTreesInt();

void sendCollectStats( )
{
   if (pGlblJamStatCallBack != 0)
   {
      pGlblJamStatCallBack(0, glblFileCount.value(), cs.lines, (uint)(cs.totalbytes/1000000UL), glblFileCount.skipped(), glblFileCount.skipInfo());
   }
}



#ifndef USE_SFK_BASE

#ifdef SFK_W64
void widetochar(ushort *ain, char *aout)
{
   int i=0;
   for (; ain[i]; i++)
      aout[i] = (char)ain[i];
   aout[i] = '\0';
}

int walkFilesW(ushort *atop, int lLevel)
{
   if (bGlblEscape)
      return 0;

   if (cs.debug) printf("] wfl: %s\n", dataAsTraceW(atop));

   int lRC = 0;

   ushort atopmask[1024];
   ushort aabsfile[1024];
   char   szrelfile[1024];

   {
      int i=0;
      for (; atop[i]; i++)
         atopmask[i] = atop[i];
      atopmask[i++] = '\\';
      atopmask[i++] = '*';
      atopmask[i] = '\0';
   }

   sfkfinddata64_t odata;

   bool bfirst = 1;
   intptr_t pdir = 0;

   while (1)
   {
      if (bfirst) {
         bfirst = 0;
         pdir = _wfindfirst64((const wchar_t *)atopmask, &odata);
         if (pdir == -1) {
            if (cs.debug) printf("] wfl: stop\n");
            return 0;
         }
      } else {
         int isubrc = _wfindnext64(pdir, &odata);
         if (isubrc)
            break;
      }

      ushort *arelfile = (ushort*)odata.name;

      // build absolute name
      int i=0;
      for (; atop[i]; i++)
         aabsfile[i] = atop[i];
      aabsfile[i++] = '\\';
      for (int k=0; arelfile[k]; k++)
         aabsfile[i++] = arelfile[k];
      aabsfile[i++] = 0;
      widetochar(arelfile, szrelfile);

      if (odata.attrib & 0x10)
      {
         // sub folder
         if (arelfile[0]=='.' && arelfile[1]==0)
            continue;
         if (arelfile[0]=='.' && arelfile[1]=='.' && arelfile[2]==0)
            continue;
         if (!cs.withsub(lLevel+1))
            continue;

         lRC = walkFilesW(aabsfile, lLevel+1);

         if (cs.stopTree(lRC))
            break;

         lRC = 0;

         continue;
      }
      else if (matchesFileMask(szrelfile, 0, 1))
      {
         lRC = execFixFile(aabsfile, &odata);
         // todo: rc mapping?
      }
   }

   _findclose(pdir);

   return lRC;
}

int walkAllTreesW(int nFunc)
{
   nGlblFunc = nFunc;

   int lRC = 0;
   bool bsilent = 0;

   for (int nDir=0; glblFileSet.hasRoot(nDir); nDir++)
   {
      if (userInterrupt())
         break;

      char *pszTree = glblFileSet.setCurrentRoot(nDir);
      if (cs.debug) printf("] wat: tree %s\n", pszTree);

      uchar *pszRoot = (uchar*)glblFileSet.root(1); // returns 0 if none
      ushort aroot[1024];
      int i=0;
      for (; pszRoot[i]; i++)
         aroot[i] = pszRoot[i];
      aroot[i] = 0;

      lRC = walkFilesW(aroot, 0);

      if (cs.stopTree(lRC, &bsilent)) {
         if (bsilent)
            return 0;
         nGlblError = 1;
         return 9;
      }
      lRC = 0;

      if (cs.stopTree(lRC, &bsilent)) {
         if (bsilent)
            return 0;
         nGlblError = 1;
         return 9;
      }
      lRC = 0;

      if (bGlblEscape)
         break;
   }

   return lRC;
}
#endif

#endif // USE_SFK_BASE

// used by matchesNormName only to check .ext dir masks
bool dirExtEndMatch(char *pszHay, char *pszPat)
{
   // hay1: ".svn/"
   // pat1: ".svn"
   // pat2: ".svn/"
   int npatlen = strlen(pszPat);
   if (npatlen < 1) return 0; // shouldn't happen
   // case ".svn/" == ".svn/"
   if (pszPat[npatlen-1] == glblPathChar)
      return mystricmp(pszHay, pszPat) ? 0 : 1;
   // pat differs in length to hay,
   // case ".svn/" == ".svn"
   int nhaylen = strlen(pszHay);
   if (nhaylen != npatlen+1)
      return 0;
   if (mystrnicmp(pszHay, pszPat, npatlen))
      return 0;
   if (pszHay[npatlen] != glblPathChar)
      return 0;
   return 1;
}

int normalizePath(char *pszSrc, char *pszDst, int iMaxDst)
{
   if (iMaxDst < 100)
      return 9+perr("buffer overflow #213461 %d", iMaxDst);

   char *pszMaxDst = pszDst+iMaxDst-10;

   pszDst[0] = '\0';

   int iNameLen = strlen(pszSrc);

   if (iMaxDst < iNameLen+4)
      return 10+perr("buffer overflow #213462 %d %d", iMaxDst, iNameLen);

   if (pszSrc[0] != glblPathChar)
      strcat(pszDst, glblPathStr);

   if (iNameLen > 0)
   {
      int iAppendPos = strlen(pszDst);

      if (pszDst+iAppendPos+iNameLen > pszMaxDst)
         return 11; // safety

      memcpy(pszDst+iAppendPos, pszSrc, iNameLen);
      pszDst[iAppendPos+iNameLen] = '\0';
   }

   if (iNameLen == 0 || pszSrc[iNameLen-1] != glblPathChar)
      strcat(pszDst, glblPathStr);

   return 0;
}

// IN: pszStr must be /thedir/file.txt/ normalized
//     with surrounding slashes. see normalizePath.
//     always use nflags = 1 or 3 then.
bool matchesNormName(char *pszStr, char *pszMask,
   bool *rPartMatch = 0, int nFlags = 0,
   int *pHitPos = 0, int *pHitLen = 0
   )
{
    if (cs.verbose >= 4)
       printf("\nmname %s %s flags=%u\n",pszStr,pszMask,nFlags);

    // flags, bit 0: DISABLE start of name comparison
    bool bEnableSNC = ((nFlags & 1) == 0);

    // flags, bit 1: PATH matching mode i/o FILE.
    bool bPathMatch =  (nFlags & 2) ? 1 : 0;

    // flags, bit 2: matchesDirMask mode, no ".ext" comparison.
    bool bMDirMask  =  (nFlags & 4) ? 1 : 0;

    // printf("SNC=%d PM=%d MDirMask=%d\n",bEnableSNC,bPathMatch,bMDirMask);

    //  foo         -> thefoosys.txt
    //  *foo*bar*   -> thefooanybar.txt
    //  .txt        -> only .txt files
    //  !.txt       -> all except .txt files
    //  * anywhere  -> ignore . position

    if (rPartMatch) *rPartMatch = false;

    szMatchBuf[0] = '\0';

    int nMinHitPos = -1;
    int nMaxHitLen =  0;

    char *pstr1  = pszStr;
    char *pmsk1  = pszMask;
    char *pmsk2  = pmsk1;
    bool  bneg   = 0;
    bool  bimt   = true; // inner match
    int  nmpos  = 0;
    int  nlen   = 0;
    bool  bwild2 = 0;

    while (*pmsk1)
    {
        char c = *pmsk1;
        bool bIsLastChr = (*(pmsk1+1) == '\0');

        // initial negation
        if (!nmpos && isNotChar(c))
            { bneg=1; pmsk1++; pmsk2=pmsk1; nmpos++; continue; }
        nmpos++;

        // identify tokens between *
        bool bwild = isWildChar(c); // checks for *, but not ?
        if (bwild || bIsLastChr)
        {
            // have mask part from pmsk2 to pmsk1-1
            nlen = pmsk1 - pmsk2;
            if (!bwild) nlen++; // do not exlude * then

            // "*" as first character of mask?
            if (nlen <= 0) {
               if (bPathMatch) {
                  pmsk1++; pmsk2=pmsk1; bwild2=1; continue;
               } else {
                  pmsk1++; pmsk2=pmsk1; continue;
               }
            }

            // have mask part from pmsk2 with length nlen
            if (nlen > MAX_MATCH_BUF-2) {
               if (cs.verbose >= 3) printf("0 = MATCH1 (msk %.100s, str %s) r2\n",pmsk2,pszStr);
               return false;
            }
            memcpy(szMatchBuf, pmsk2, nlen);
            szMatchBuf[nlen] = '\0';
            if (cs.verbose >= 4) printf("PART %s neg %d\n", szMatchBuf, bneg);

            // special case ".ext" part at end of mask?
            if (!bwild && !bwild2 && szMatchBuf[0] == '.')
            {
                // compare mask: szMatchBuf e.g. ".1.txt"
                // against end of: pstr1    e.g. "foo.1.txt"
                int nMaskLen = strlen(szMatchBuf);
                int nNameLen = strlen(pstr1);

                // FIX: R157: no longer use strrchr(pstr1, '.')
                char *psz5 = 0;
                if (endsWithPathChar(szMatchBuf, glblUPatMode)) {
                   if (nMaskLen <= nNameLen) {
                      psz5 = pstr1 + nNameLen - nMaskLen;
                      if (*psz5 != '.') psz5 = 0;
                   }
                } else {
                   // FIX 163R3: .txt <-> tmp1.txt/
                   // fix includes strnicmp below i/o stricmp
                   if (nMaskLen+1 <= nNameLen) {
                      psz5 = pstr1 + nNameLen - (nMaskLen+1);
                      if (*psz5 != '.') psz5 = 0;
                   }
                }
                bool bmatch1 = false;
                if (!psz5)
                    bmatch1 = false;
                else
                if (bMDirMask)
                    bmatch1 = dirExtEndMatch(psz5, szMatchBuf);
                else
                    bmatch1 = !mystrnicmp(psz5, szMatchBuf, nMaskLen);
                if (bmatch1 && rPartMatch) *rPartMatch = true;
                // final match decision by extension:
                if (cs.verbose >= 3) printf("%d = MATCH2 (msk %s, str %s) %d^%d r3 pcmp=%p\n",bneg^bmatch1,szMatchBuf,pszStr,bneg,bmatch1,psz5);
                return bneg ^ bmatch1;
            }

            int nHitPos = 0;

            // string cursor is pstr1
            bool bmatch2 = false;

            if (bEnableSNC && isUniPathChar(szMatchBuf[0]) && (szMatchBuf[1] != '\0'))
            {
               // start of name comparison:
               if (mystrstriq4k(pstr1, &szMatchBuf[1], &nHitPos))
                  if (nHitPos == 0) {
                     bmatch2 = true;
                     if (pHitPos) {
                        nMinHitPos = 0;
                        nMaxHitLen = strlen(&szMatchBuf[1]);
                     }
                  }
            }
            else
            {
               // normal within-name comparison:
               if (mystrstriq4k(pstr1, szMatchBuf, &nHitPos)) {
                   // part matches:
                   if (rPartMatch) *rPartMatch = true;
                   if (cs.verbose >= 4) printf("CHK0 bneg %d (msk %s,str %s)\n",bneg,szMatchBuf,pszStr);
                   // if (bneg) return false;
                   // else continue, no mismatch yet.
                   if (pHitPos) {
                      int nAbsPos1 = (pstr1-pszStr)+nHitPos;
                      if (nMinHitPos < 0) nMinHitPos = nAbsPos1;
                      int nAbsPos2 = nAbsPos1 + strlen(szMatchBuf);
                      nMaxHitLen = nAbsPos2 - nMinHitPos;
                   }
                   // have to step past hit in string.
                   pstr1 += nHitPos + strlen(szMatchBuf);
                   pmsk1++; pmsk2=pmsk1;
                   continue;
               }
               // else fall through, bmatch2 hasn't been raised.
            }

            if (!bmatch2)
            {
                // part mismatch:
                bimt = false; // no inner match
                if (!bneg) {
                   if (cs.verbose >= 3) printf("0 = MATCH3 (msk %s, str %s) r4\n",szMatchBuf,pszStr);
                   return false;
                }
                if (cs.verbose >= 4) printf("CHK2 bneg %d (msk %s,str %s)\n",bneg,szMatchBuf,pszStr);
                // else continue, no negation match yet
                // e.g. "!the*foo" over "bigtest.txt";
                // do not step pstr1 at all.
                if (cs.verbose >= 4) printf("CNT2 bneg %d\n",bneg);
                pmsk1++; pmsk2=pmsk1; continue;
            } else {
                pmsk1++; pmsk2=pmsk1; continue;
            }
        }
        // continue on characters within tokens
        pmsk1++;
    }
    // the string past "!" (if any) matches:
    if (cs.verbose >= 3) printf("%d = MATCH4 (msk \"%s\", str \"%s\") %d^%d\n",bimt^bneg,szMatchBuf,pszStr,bimt,bneg);
    bool brc = bimt ^ bneg;
    if (brc && pHitPos && (nMinHitPos >= 0) && (nMaxHitLen > 0)) {
       if (pHitPos) *pHitPos = nMinHitPos;
       if (pHitLen) *pHitLen = nMaxHitLen;
    }
    return brc;
}

// RC 0 : no match
// RC 1 : match, but just by wildcard
// RC 2 : match by non wildcard pattern
// RC 3 : matches by .ext // sfk1944
int matchesFileMask(char *pszFile, char *pszInfoAbsName, int iFromInfo)
{
   int iRC = 1;
 
   // build normalized filename
   //    foo.txt     -> /foo.txt/
   //    foo\bar.txt -> /foo/bar.txt/
   int iNameLen = strlen(pszFile);
 
   char *pszNormSubName = new char[iNameLen + 10];
   CharAutoDel odel(pszNormSubName);

   pszNormSubName[0] = '\0';
   if (pszFile[0] != glblPathChar)
      strcat(pszNormSubName, glblPathStr);
   if (iNameLen > 0) {
      int iAppendPos = strlen(pszNormSubName);
      memcpy(pszNormSubName+iAppendPos, pszFile, iNameLen);
      pszNormSubName[iAppendPos+iNameLen] = '\0';
   }
   if (iNameLen == 0 || pszFile[iNameLen-1] != glblPathChar)
      strcat(pszNormSubName, glblPathStr);

   int iNumberOfWhiteMasks    = 0;
   int iNumberOfWhiteMatches  = 0;
   int iNumberOfBlackMasks    = 0;
   int iNumberOfWildMasks     = 0;

   Array &rMasks = glblFileSet.fileMasks();
   for (int i=0; rMasks.isStringSet(i); i++)
   {
      char *pszMask = rMasks.getString(i);

      if (isNotChar(pszMask[0]))
      {
         iNumberOfBlackMasks++;
         if (matchesNormName(pszNormSubName, pszMask+1, 0, 3)) { // ,3: no start-of-name + cmp-path
            if (nGlblTraceSel & 1) {
               setTextColor(nGlblTraceExcColor);
               info.print("file-exclud: %s due to \"%s\" (from=%d)\n", pszNormSubName, pszMask, iFromInfo);
               setTextColor(-1);
            }
            return 0;
         } else {
            if (cs.verbose >= 4)
               printf("1 = msbm (msk %s, str %s) (from=%d)\n", pszMask, pszNormSubName, iFromInfo);
         }
      }
      else
      {
         iNumberOfWhiteMasks++;
         if (matchesNormName(pszNormSubName, pszMask, 0, 3)) { // ,3: no start-of-name + cmp-path
            iNumberOfWhiteMatches++;
            if (!containsWildCards(pszMask)) {
               iRC = 2;
               if (pszMask[0] == '.')  // sfk1944
                  iRC = 3;
            }
            if (nGlblTraceSel & 1) {
               setTextColor(nGlblTraceExcColor);
               info.print("file-wmatch: %s to \"%s\" (from=%d)\n", pszNormSubName, pszMask, iFromInfo);
               setTextColor(-1);
            }
         } else {
            if (cs.verbose >= 4)
               printf("0 = mswm (msk %s, str %s) (from=%d)\n", pszMask, pszNormSubName, iFromInfo);
         }
      }
   }

   // if any white masks given, at least one or all must match
   if (cs.fileMaskAndMatch) {
      if (iNumberOfWhiteMasks > 0 && iNumberOfWhiteMatches < iNumberOfWhiteMasks)
         iRC = 0;
   } else {
      if (iNumberOfWhiteMasks > 0 && iNumberOfWhiteMatches < 1)
         iRC = 0;
   }

   if (nGlblTraceSel & 1) {
      if (iRC) {
         setTextColor(nGlblTraceIncColor);
         info.print("file-keep  : %s (wmask=%d/%d bmask=0/%d) (from=%d)\n", pszNormSubName, iNumberOfWhiteMatches, iNumberOfWhiteMasks, iNumberOfBlackMasks, iFromInfo);
         // mtklog(("mdm: include %s", pszStr));
      } else {
         setTextColor(nGlblTraceExcColor);
         info.print("file-exclud: %s (wmask=%d/%d bmask=0/%d) (from=%d)\n", pszNormSubName, iNumberOfWhiteMatches, iNumberOfWhiteMasks, iNumberOfBlackMasks, iFromInfo);
         // mtklog(("mdm: exclude %s", pszStr));
      }
      setTextColor(-1);
   }

   // mtklog(("mdm: %d for %s", iRC, pszStr));

   return iRC;
}

bool matchesDirMask(char *pszFullPath, bool bTakeFullPath, bool bApplyWhiteMasks, int iFromInfo)
{_p("sf.mtchdir")

   bool bRC = 1;

   // build normalized path name
   //    include  -> /include/
   //    \foo\bar -> /bar/
   // if input path is an absolute filename
   //    include\foo.hpp -> /include/
   int iPathLen = strlen(pszFullPath);
   if (!bTakeFullPath) {
      char *pszRelName = strrchr(pszFullPath, glblPathChar);
      if (!pszRelName)
         iPathLen = 0;
      else
         iPathLen = pszRelName - pszFullPath;
   }

   char *pszNormSubName = new char[iPathLen + 10];
   CharAutoDel odel(pszNormSubName);

   pszNormSubName[0] = '\0';
   if (pszFullPath[0] != glblPathChar)
      strcat(pszNormSubName, glblPathStr);
   if (iPathLen > 0) {
      int iAppendPos = strlen(pszNormSubName);
      memcpy(pszNormSubName+iAppendPos, pszFullPath, iPathLen);
      pszNormSubName[iAppendPos+iPathLen] = '\0';
   }
   if (iPathLen == 0 || pszFullPath[iPathLen-1] != glblPathChar)
      strcat(pszNormSubName, glblPathStr);
 
   int iNumberOfWhiteMasks    = 0;
   int iNumberOfWhiteMatches  = 0;
   int iNumberOfBlackMasks    = 0;
   int iNumberOfWildMasks     = 0;

   Array &rMasks = glblFileSet.dirMasks();
   for (int i=0; rMasks.isStringSet(i); i++)
   {
      char *pszMask = rMasks.getString(i);

      if (isNotChar(pszMask[0]))
      {
         // black masks are applied asap, both during directory travel
         // and later when checking file paths.
         iNumberOfBlackMasks++;
         if (matchesNormName(pszNormSubName, pszMask+1, 0, 3)) { // ,3: no start-of-name + cmp-path
            if (nGlblTraceSel & 1) {
               setTextColor(nGlblTraceExcColor);
               info.print("dir-exclude: %s due to \"%s\" (from=%d)\n", pszNormSubName, pszMask, iFromInfo);
               setTextColor(-1);
            }
            return 0;
         } else {
            if (cs.verbose >= 4)
               printf("1 = msbm (msk %s, str %s) (from=%d)\n", pszMask, pszNormSubName, iFromInfo);
         }
      }
      else
      if (bApplyWhiteMasks)
      {
         // white masks are applied on file path checking,
         // but not on directory travel.
         iNumberOfWhiteMasks++;
         if (matchesNormName(pszNormSubName, pszMask, 0, 3)) { // ,3: no start-of-name + cmp-path
            iNumberOfWhiteMatches++;
            if (nGlblTraceSel & 1) {
               setTextColor(nGlblTraceExcColor);
               info.print("dir-wmatch : %s to \"%s\" (from=%d)\n", pszNormSubName, pszMask, iFromInfo);
               setTextColor(-1);
            }
         } else {
            if (cs.verbose >= 4)
               printf("0 = mswm (msk %s, str %s) (from=%d)\n", pszMask, pszNormSubName, iFromInfo);
         }
      }
   }
 
   // if any white masks given, at least one or all must match
   if (cs.dirMaskAndMatch) {
      if (iNumberOfWhiteMasks > 0 && iNumberOfWhiteMatches < iNumberOfWhiteMasks)
         bRC = 0;
   } else {
      if (iNumberOfWhiteMasks > 0 && iNumberOfWhiteMatches < 1)
         bRC = 0;
   }
 
   if (nGlblTraceSel & 1) {
      if (bRC) {
         setTextColor(nGlblTraceIncColor);
         info.print("dir-keep   : %s (wmask=%d/%d bmask=0/%d) (from=%d)\n", pszNormSubName, iNumberOfWhiteMatches, iNumberOfWhiteMasks, iNumberOfBlackMasks, iFromInfo);
         // mtklog(("mdm: include %s", pszStr));
      } else {
         setTextColor(nGlblTraceExcColor);
         info.print("dir-exclude: %s (wmask=%d/%d bmask=0/%d) (from=%d)\n", pszNormSubName, iNumberOfWhiteMatches, iNumberOfWhiteMasks, iNumberOfBlackMasks, iFromInfo);
         // mtklog(("mdm: exclude %s", pszStr));
      }
      setTextColor(-1);
   }

   // mtklog(("mdm: %d for %s", bRC, pszStr));

   return bRC;
}

bool isQuoteChar(char c)
{
   switch ((uchar)c)
   {
      case '\"': return 1;
      case '\'': return 1;
      case 0x60: return 1;
      case 0x91: return 1;
      case 0x92: return 1;
      case 0x93: return 1;
      case 0x94: return 1;
   }
   return false;
}

int onRunExpression(char *psz1, int &lExpLength, bool &bquot, bool &btext);

enum eRunExpressions
{
   erun_path      = 0,
   erun_file      = 1,
   erun_upath     = 2,
   erun_ufile     = 3,
   erun_relfile   = 4,
   erun_base      = 5,
   erun_ext       = 6,
   erun_since     = 7,
   erun_text      = 8,
   erun_relpath   = 9,   // sfk1973
   erun_targ      = 10,  // sfk182 with -tomake only
   erun_outext    = 11   // sfk1973 with [f]pic only
};

// out   : pDstBuf with MAX_LINE_LEN
// uses  : szLineBuf
// rc    : 0 if replacements done, 1 if none found,
//         >= 9 on format error
int renderOutMask(char *pDstBuf, Coi *pcoi, char *pszMask, cchar *pszCmd, bool bUniPath=0)
{__
   if (cs.debug)
      printf("[render command mask: %s]\n", pszMask);

   if (cs.upath)
      bUniPath = 1;

   char   inPathChar = bUniPath ? '/'  : glblPathChar;
   cchar *inDotSlash = bUniPath ? "./" : glblDotSlash;

   char *pszInFile   = pcoi->name();
   char *pszFileName = pszInFile;

   if (!strncmp(pszFileName, inDotSlash, 2))
      pszFileName += 2;

   // relativize filename. todo: used at all?
   if (cs.rootrelname)
      pszFileName = pcoi->rootRelName();

   // copy command template to command buffer
   copyFormStr(pDstBuf, MAX_LINE_LEN, pszMask, strlen(pszMask));

   // preparations
   char *pszRelFilename = 0;
   if (bUniPath) {
      pszRelFilename = strrchr(pszFileName, '/');
      if (pszRelFilename)
         pszRelFilename++;
      else
         pszRelFilename = pszFileName;
   } else {
      pszRelFilename = relativeFilename(pszFileName);
   }

   bool bDoneAny = 0;
   bool bUsingText = false;

   char *psz1 = pDstBuf;
   int   iexp = 0;

   #ifdef SFK_BOTH_RUNCHARS
   while (*psz1!=0 && *psz1!='#' && *psz1!='$') psz1++;
   #else
   while (*psz1!=0 && *psz1!='#') psz1++;
   #endif

   while (psz1 && *psz1)
   {
      int lTokenLen = 0;
      bool bQuoted  = false;
      // sfk181: support for $$ or ## escape
      if (cs.strict!=0 && psz1[1]==psz1[0]) {
         memmove(psz1,psz1+1,strlen(psz1+1)+1);
         psz1++;
      }
      else
      switch (iexp = onRunExpression(psz1, lTokenLen, bQuoted, bUsingText))
      {
         case erun_file:
         case erun_text:
         case erun_ufile:
         {
            // replace absolute filename
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
             strcat(szLineBuf, pszFileName);
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            // apply unix path conversion?
            if (iexp == erun_ufile)
               for (int i=0; pDstBuf[i]; i++)
                  if (pDstBuf[i]=='\\')
                     pDstBuf[i]='/';
            bDoneAny = 1;
            break;
         }

         case erun_relfile:
         {
            // replace relative filename
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
             strcat(szLineBuf, pszRelFilename);
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            bDoneAny = 1;
            break;
         }

         case erun_path:
         case erun_upath:
         {
            // replace filename path
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
            char *psz3 = strrchr(pszFileName, inPathChar);
            if (psz3 && (psz3-pszFileName)>0)
               strncat(szLineBuf, pszFileName, psz3-pszFileName);
            else
               strcat(szLineBuf, ".");
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            // apply unix path conversion?
            if (iexp == erun_upath)
               for (int i=0; pDstBuf[i]; i++)
                  if (pDstBuf[i]=='\\')
                     pDstBuf[i]='/';
            bDoneAny = 1;
            break;
         }

         case erun_relpath:   // sfk1973
         {
            // replace filename path
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");

            char *psz3 = pcoi->rootRelName();
            char *psz4 = strrchr(psz3, inPathChar);
            if (psz4 && (psz4-psz3)>0
                && strlen(szLineBuf)+strlen(psz3) < MAX_LINE_LEN)
               strncat(szLineBuf, psz3, psz4-psz3);

            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            // apply unix path conversion?
            if (iexp == erun_upath)
               for (int i=0; pDstBuf[i]; i++)
                  if (pDstBuf[i]=='\\')
                     pDstBuf[i]='/';
            bDoneAny = 1;
            break;
         }

         case erun_base:
         {
            // replace file base name, without ".ext"
            // have to use relative filename for this.
            // note: ".afile" has ".afile" as base
            // note: "afile.int.longext" has ".longext" as ext
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
            char *psz3 = strrchr(pszRelFilename, '.');
            if (psz3 && (psz3 > pszRelFilename)) {
               // can identify extension
               strncat(szLineBuf, pszRelFilename, psz3-pszRelFilename);
            } else {
               // cannot identify extension
               strcat(szLineBuf, pszRelFilename);
            }
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            bDoneAny = 1;
            break;
         }

         case erun_ext:
         {
            // replace file extension
            // have to use relative filename for this.
            // note: ".afile" has ".afile" as base
            // note: "afile.int.longext" has ".longext" as ext
            // note: "afile." has "" as extension
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
            char *psz3 = strrchr(pszRelFilename, '.');
            if (psz3 && (psz3 > pszRelFilename)) {
               // can identify extension, zero length accepted
               strcat(szLineBuf, psz3+1);
            } else {
               // cannot identify extension, leave empty
            }
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            bDoneAny = 1;
            break;
         }

         case erun_outext:
         {
            // replace $outext
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
            if (pcoi)
               strcat(szLineBuf, pcoi->szClOutExt);
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            bDoneAny = 1;
            break;
         }

         case erun_since:
         case erun_targ:
         {
            // replace absolute sincedir filename incl. root
            char *pszSinceFile = pcoi->ref(1); // returns null if none
            if (!pszSinceFile) {
               if (iexp == erun_targ) {
                  perr("missing reference name, cannot replace \"targ\" token.\n");
                  pinf("-tomake may not have been specified before.\n");
                  pinf("a previous command may not support passing -tomake names.\n");
               } else {
                  perr("missing reference name, cannot replace \"since\" token.\n");
                  pinf("-sincedir/add/diff may not have been specified before.\n");
                  pinf("a previous command may not support passing -since names.\n");
               }
               return 9;
            }
            memset(szLineBuf, 0, sizeof(szLineBuf));
            strncpy(szLineBuf, pDstBuf, psz1-pDstBuf);
            // middle
            char *psz2 = psz1+lTokenLen;
            if (bQuoted) strcat(szLineBuf, "\"");
             strcat(szLineBuf, pszSinceFile);
            if (bQuoted) strcat(szLineBuf, "\"");
            // remember position past insert
            psz1 = pDstBuf+strlen(szLineBuf);
            // right
            strcat(szLineBuf, psz2);
            // copy back result
            mystrcopy(pDstBuf, szLineBuf, MAX_LINE_LEN);
            bDoneAny = 1;
            break;
         }

         default:
         {
            if (cs.strict) {
               perr("unknown token in command mask: %s\n",psz1);
               if (cs.usevars)
                  pinf("use %c%.20s or %c%c%.20s to escape the text.\n",
                     *psz1,psz1, *psz1,*psz1,psz1);
               else
                  pinf("use %c%.20s ... to escape the text.\n",*psz1,psz1);
               static bool btold=0;
               if (cs.strict<2 && btold==0) {
                  btold=1;
                  pinf("use option -nostrict to ignore. note that this option\n");
                  pinf("may cause a script failure with future sfk versions.\n");
               }
               return 10;
            } else {
               if (cs.debug)
                  printf("[skipping no-mask token: %s]\n",psz1);
            }
            psz1++;
            break;
         }

      }  // end switch

      // find next potential token, if any
      #ifdef SFK_BOTH_RUNCHARS
      while (*psz1!=0 && *psz1!='#' && *psz1!='$') psz1++;
      #else
      while (*psz1!=0 && *psz1!='#') psz1++;
      #endif
   }

   // user error detection:
   // if filename contains blanks,
   if (!bUsingText && !cs.quiet && !cs.noinfo) {
      if (strchr(pszInFile, ' ')) {
         cs.blankRunFiles++;
         // then count quotes in produced expression
         int nlen  = strlen(pDstBuf);
         int nquot = 0;
         for (int i=0; i<nlen; i++)
            if (isQuoteChar(pDstBuf[i]))
               nquot++;
         if (nquot == 0) cs.badNameForm |= 1;
      }
      if (!bUniPath && strchr(pszInFile, glblWrongPChar)) {
         cs.wrongpcRunFiles++;
         cs.badNameForm |= 2;
      }
   }

   return bDoneAny ? 0 : 1;
}

int printEcho(uint nflags, const char *pszFormat, ...)
{__
   #ifdef SFKPRINTREDIR
   bool bcollect = chain.colany();
   #else
   bool bcollect = chain.coldata;
   #endif

   bool bAddToCurLine = (nflags & 1) ? 1 : 0;
   bool bTell         = (nflags & 2) ? 1 : 0;
   bool bEcho         = bTell ? 0 : 1;
   bool bToTerm       = (nflags & 4) ? 1 : 0;

   if (bToTerm)
      bcollect = 0;

   va_list argList;
   va_start(argList, pszFormat);

   ::vsnprintf(szPrintBuf1, sizeof(szPrintBuf1)-10, pszFormat, argList);
   szPrintBuf1[sizeof(szPrintBuf1)-10] = '\0';

   char *pszSrc = szPrintBuf1;
   int iDst = 0;
   char nAttr = ' ';
   bool bResetOnLF = 0;
   while (*pszSrc && (iDst < (int)sizeof(szPrintBuf2)-10))
   {
      if (!strncmp(pszSrc, "[["    , 2)) {
         pszSrc += 2;
         szPrintBuf2[iDst] = '[';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      if (!strncmp(pszSrc, "]]"    , 2)) {
         pszSrc += 2;
         szPrintBuf2[iDst] = ']';
         szPrintAttr[iDst] = nAttr;
         iDst++;
      }
      else

      // base colors are bright or dark depending on their first name char
      if (!mystrncmp(pszSrc, "[red]"    , 5)) { nAttr = pszSrc[1]; pszSrc += 5; } else
      if (!mystrncmp(pszSrc, "[green]"  , 7)) { nAttr = pszSrc[1]; pszSrc += 7; } else
      if (!mystrncmp(pszSrc, "[blue]"   , 6)) { nAttr = pszSrc[1]; pszSrc += 6; } else
      if (!mystrncmp(pszSrc, "[yellow]" , 8)) { nAttr = pszSrc[1]; pszSrc += 8; } else
      if (!mystrncmp(pszSrc, "[cyan]"   , 6)) { nAttr = pszSrc[1]; pszSrc += 6; } else
      if (!mystrncmp(pszSrc, "[magenta]", 9)) { nAttr = pszSrc[1]; pszSrc += 9; } else
      // inofficial: does not work with a white background under Windows
      if (!mystrncmp(pszSrc, "[white]"  , 7)) { nAttr = 'v';       pszSrc += 7; } else

      if (!strncmp(pszSrc, "[def]"    , 5)) { pszSrc += 5; nAttr = ' '; }

      else
      {
         szPrintBuf2[iDst] = *pszSrc;
         szPrintAttr[iDst] = nAttr;
         if (*pszSrc == '\n' && bResetOnLF) {
            nAttr = ' ';
            szPrintAttr[iDst] = nAttr;
            bResetOnLF = 0;
         }
         pszSrc++;
         iDst++;
      }
   }
   szPrintBuf2[iDst] = '\0';
   if (bEcho && bcollect) {
      if (bAddToCurLine) {
         chain.addToCurLine(szPrintBuf2, szPrintAttr, 0);
      } else {
         // mode 2: if echo text ends with "\n", add an empty line.
         chain.addLine(szPrintBuf2, szPrintAttr, 2);
      }
   } else {
      printColorText(szPrintBuf2, szPrintAttr, 0);
      if (!bAddToCurLine) printf("\n");
      fflush(stdout);
   }
   return 0;
}

// sfk1944: bGlblInSpecificProcessing -> inFileList
void setUsingFileList(int bYesNo)
{
   cs.infilelist = bYesNo;
}

BinTexter::BinTexter(Coi *pcoi)
{
   memset(this, 0, sizeof(*this));
   pClCoi = pcoi;
}

BinTexter::~BinTexter()
{
   memset(this, 0, sizeof(*this));
}

// uses szLineBuf. Result in szLineBuf2.
int BinTexter::process(int nDoWhat)
{
   int icol   = 0;
   int istate = 0;
   int iword  = 0;  // index in short word target buffer
   int nword  = 0;  // to count non-binary word length
   int ihi    = 0;
   bool bflush = 0;
   bool bisws  = 0;
   bool bwasws = 0;
   bool bisbin = 0;
   bool bishi  = 0;
   bool bispunct = 0;
   bool bhardwrap = 0;
   bool babineol = 0; // helper flag, add blank if not at end of line
   char c = 0;
   unsigned char uc = 0;
   int nLine = 0;
   int nMinWord = 1; // min word length adapted dynamically below
   int lRC = 0;

   szClOutBuf[0] = '\0';

   num  nTellTime = getCurrentTime();
   int nTellLines = 0;

   bool bbail = 0;
   while (!bbail)
   {
      int nRead = pClCoi->read(szLineBuf, sizeof(szLineBuf)-10);

      if (nRead <= 0) {
         bbail = 1;
         nRead = 1;
      }
 
      if (nTellLines++ > 1000) {
         nTellLines = 0;
         if (getCurrentTime() > nTellTime + 1000) {
            // working on the same file for 1000 msec: show status
            nTellTime = getCurrentTime();
            if (!cs.quiet && pClCoi->name()) {
               info.setAddInfo("%u files, %u dirs", cs.filesScanned, cs.dirsScanned);
               info.setStatus("scan", pClCoi->name(), 0, eKeepAdd);
            }
         }
      }

      for (int i=0; i<nRead; i++)
      {
         if (bbail) {
            c  = 0x00;
            uc = (unsigned char)c;
            bflush = 1;
         } else {
            c  = szLineBuf[i];
            uc = (unsigned char)c;
            bflush = 0;
         }

         if (c=='\n') {
            nLine++;
            bflush = 1;
            bhardwrap = 1;
         }

         if (cs.rewrap) {
            // reformatting plain text
            bisbin = (uc == 0) ? 1 : 0; // just in case
         } else {
            // reformatting binary
            bisbin = (uc < 0x20) ? 1 : 0;
            if (uc >= 0x80 && cs.binallchars == 0) // sfk190
               bisbin = 1;
            // if (cs.binallchars == 0)
            //    bishi = (uc >= 0xC0);
         }

         if (!bisbin && !bishi)
            nword++;

         if (sfkisprint(c) && !bisbin)
         {
            // printable char
            if (istate == 1) {
               // start collecting next word
               istate = 0;
               iword = 0;
               ihi   = 0;
            }
            // continue collecting current word,
            // reduce multi-whitespace sequences.
            bisws    = (c==' ' || c=='\t');
            bispunct = 0;
            if (cs.delim && strchr(cs.delim, c)) // CHG: 1703: instead of (c=='.' || c==',' || c==';');
               bispunct = 1;
            if (!(bisws && bwasws)) {
               szLineBuf2[iword++] = c;
               if (bishi)
                  ihi++;
            }
            // hard or soft word break?
            if (iword >= cs.wrapbincol)
               bflush = 1;
            else
            if ((iword >= 20) && (bisws || bispunct))
               bflush = 1;
         } else {
            // non-printable (binary) char
            if (istate == 0)  // if collecting a word
               bflush = 1;    // then flush the word
            // start skipping non-word data (binary etc.)
            istate = 1;
            bwasws = 0;
            bisws  = 0;
            bispunct = 0;
            // increment min word length needed to add to buffer.
            nMinWord = 3;
         }

         // dump current word?
         if (bflush)
         {
            bflush = 0;
            if ((iword >= nMinWord || nword >= nMinWord) || c == '\n' || bbail)
            {
               // reset "add blank if not at end of line"
               babineol = 0;

               szLineBuf2[iword] = '\0';

               // do not add blank after whitespace, punct, or End Of Data.
               // add blank after a linefeed, if not in grep mode.
               if (bisws || bispunct || bbail || c == '\n') {
                  if (nDoWhat != eBT_Grep && c == '\n')
                     if (iword > 0 || nword > 0)
                        babineol = 1;
               } else {
                  // all other cases including '\r':
                  // add blank between isolated expressions
                  babineol = 1; // if not at end of line
               }

               if (strlen(szClOutBuf) + strlen(szLineBuf2) < sizeof(szClOutBuf)-10)
                  strcat(szClOutBuf, szLineBuf2);
               // else
               //    pwarn("buffer overflow (%d/%d/%d)\n",
               //       (int)strlen(szClOutBuf), (int)strlen(szLineBuf2), (int)sizeof(szClOutBuf)-10);

               icol += (iword+1);

               // strings and jamfile: create floating text.
               // in case of grep, keep hard line breaks.
               if (nDoWhat != eBT_Grep && c == '\n')
                  c = ' '; // binary to text: wrap floating text

               if (icol >= cs.wrapbincol || c == '\n' || bbail)
               {
                  // line flush.
                  if ((lRC = processLine(szClOutBuf, nDoWhat, nLine, bhardwrap)))
                     return lRC;
                  // sfk181: option -maxlines with find
                  if (cs.maxlines>0 && nLine>cs.maxlines)
                     { bbail=1; break; }
                  bhardwrap = 0;
                  icol = 0;
                  szClOutBuf[0] = '\0';
                  nword = 0;
                  nMinWord = 1;
               }
               else
               if (babineol)
               {
                  // no line flush, further stuff will be added,
                  // and remembered to insert a blank before that.
                  strcat(szClOutBuf, " ");
               }
            }
            iword  = 0;
            ihi    = 0;
            bwasws = 0;
         }
         else
            bwasws = bisws;

         // "sane" word count reset on any binary or non-printable
         if (bisbin || bishi || (c < 0x20))
            nword  = 0;
      }
   }

   return 0;
}

// snapto optional callback functions
int (*pGlblJamCheckCallBack)(char *pszFilename) = 0;
int (*pGlblJamFileCallBack)(char *pszFilename, num &rLines, num &rBytes) = 0;
int (*pGlblJamLineCallBack)(char *pszLine, int nLineLen, bool bAddLF) = 0;
int (*pGlblJamStatCallBack)(Coi *pCoiOrNull, uint nFiles, uint nLines, uint nMBytes, uint nSkipped, char *pszSkipInfo) = 0;
int (*pGlblShowDataCallBack)(char *pszLine, int nLineLen) = 0;

// snapto dump of a single text line
int dumpJamLine(char *pszLine, int nLineLen, bool bAddLF) // len 0: zero-terminated
{
   int lRC = 0;

   if (pGlblJamLineCallBack)
      lRC = pGlblJamLineCallBack(pszLine, nLineLen, bAddLF);
   else
   if (nLineLen > 0)
      fprintf(fGlblOut, "%.*s%s", (int)nLineLen, pszLine, bAddLF?cs.addsnaplf:"");
   else {
      fputs(pszLine, fGlblOut);
      if (bAddLF) fputs(cs.addsnaplf, fGlblOut);
   }

   return lRC;
}

int BinTexter::processLine(char *pszBuf, int nDoWhat, int nLine, bool bHardWrap)
{
   int iRC = 0;

   info.cycle();

   if (nDoWhat == eBT_Print) {
      if (chain.coldata) {
         setattr(szAttrBuf3, 'f', strlen(szClOutBuf)+2, MAX_LINE_LEN);
         chain.addLine(szClOutBuf, szAttrBuf3);
      } else {
         info.print("%s\n", szClOutBuf);
      }
   }
   else
   if (nDoWhat == eBT_JamFile)
   {
      int dumpJamLine(char *pszLine, int nLineLen, bool bAddLF); // len 0: zero-terminated

      // strip empty lines from binary text:
      if (!strlen(szClOutBuf) || szClOutBuf[0] == '\n')
         return 0;

      int lRC = dumpJamLine(szClOutBuf, 0, 1);

      cs.totalbytes += strlen(szClOutBuf);
      cs.lines++;

      // only for callback: check per line if stat update is required.
      // it doesn't matter that nLines is actually counted past call to dumpJamLine.
      if (pGlblJamStatCallBack && glblFileCount.checkTime())
      {
         lRC |= pGlblJamStatCallBack(pClCoi, glblFileCount.value(), cs.lines, (uint)(cs.totalbytes/1000000UL), glblFileCount.skipped(), glblFileCount.skipInfo());
         // NO printf output here! BinTexter is also used by grep, strings.
      }

      return lRC;
   }
   else
   if (nDoWhat == eBT_Grep)
   {
      // 1. count no. of hits across current AND last line
      int nMatchCur = 0;
      int nMatchPre = 0;
      int nGrepPat  = glblGrepPat.numberOfEntries();
      int iHitOff   = 0;
      for (int i=0; ((nMatchCur+nMatchPre) < nGrepPat) && (i<nGrepPat); i++)
      {
         if (mystrhit((char*)pszBuf, glblGrepPat.getString(i), cs.usecase, &iHitOff)) {
            // FIX: 1703: accept hit in current line only if in front halve,
            // otherwise truncation costs are too high. hits in rear halve
            // are moved to previous record.
            if (cs.joinlines == 0 || iHitOff < cs.wrapbincol/2) {
               nMatchCur++;
               continue;
            }
            // else fall through and check previous record
         }
         if (szClLastLine[0] && mystrhit(szClLastLine, glblGrepPat.getString(i), cs.usecase, 0))
            nMatchPre++;
      }

      // 2. if ALL pattern parts have a match somewhere, list both lines
      if ((nMatchCur+nMatchPre) == nGrepPat)
      {
         // found a matching file
         cs.anymatches = 1;

         if (chain.colfiles) {
            // filenames only, as filename chain
            // TODO: set root as glblFileSet.root()?
            chain.addFile(*pClCoi); // is copied
            return 1; // end read loop above
         }

         if (cs.useJustNames || cs.useNotNames) {
            // filenames only, as text data, NOT prefixing
            // further text data, therefore NOT including ":file "
            if (chain.coldata) {
               sprintf(szLineBuf3, "%s", pClCoi->name());
               // note: +view scans extended end of attribute line
               //       to identify 'f'ile header lines, therefore +2:
               setattr(szAttrBuf3, 'f', strlen(szLineBuf3)+2, MAX_LINE_LEN);
               chain.addLine(szLineBuf3, szAttrBuf3);
            } else {
               info.print("%s\n", pClCoi->name());
            }
            return 1; // end read loop above
         }

         // actually dump the content:

         // list filename first
         if (!bClDumpedFileName && !cs.nonames && !cs.pure) {
            bClDumpedFileName = 1;
            if (chain.coldata) {
               sprintf(szLineBuf3, ":file %s", pClCoi->name());
               // note: +view scans extended end of attribute line
               //       to identify 'f'ile header lines, therefore +2:
               setattr(szAttrBuf3, 'f', strlen(szLineBuf3)+2, MAX_LINE_LEN);
               chain.addLine(szLineBuf3, szAttrBuf3);
            } else {
               setTextColor(nGlblFileColor);
               info.print("%s :\n", pClCoi->name());
               setTextColor(-1);
            }
         }

         // create coloured display of hits of PREVIOUS line
         bool bskipcur = 0;
         if (!cs.justrc && nMatchPre)
         {
            // 1703: auto join of split result
            if (cs.joinlines)
            {
               int icur = strlen(szClLastLine);
               int irem = ((int)sizeof(szClLastLine)) - icur;
               int iadd = strlen(pszBuf);
               if (iadd < irem)
               {
                  memcpy(szClLastLine+icur, pszBuf, iadd);
                  szClLastLine[icur+iadd] = '\0';
                  bskipcur = 1;
               }
            }

            char *pszTmp = szClLastLine;
            bool bPrefixed = 0;

            memset(szClAttBuf, ' ', sizeof(szClAttBuf));
            szClAttBuf[sizeof(szClAttBuf)-1] = '\0';
            char csla = cs.joinlines ? ' ' : '/';
            char asla = cs.joinlines ? ' ' : 'p';
            if (bGlblGrepLineNum)
               sprintf(szClPreBuf, " %c %04u ", csla, (nLine > 0) ? (nLine-1) : nLine);
            else
               sprintf(szClPreBuf, " %c ", csla);
            szClAttBuf[1] = asla;
 
            int iMargin = 1;
            if (!cs.pure && !cs.noind) {
               if (chain.coldata) {
                  // FIX: 163: create new record here
                  chain.addLine(szClPreBuf, szClAttBuf);
                  bPrefixed = 1;
               } else {
                  printColorText(szClPreBuf, szClAttBuf, 0); // w/o LF
               }
               iMargin += strlen(szClPreBuf);
            }

            int iMinHitOff = -1;
            int iMaxHitOff = -1;

            memset(szClAttBuf, ' ', sizeof(szClAttBuf));
            szClAttBuf[sizeof(szClAttBuf)-1] = '\0';
            for (int k=0; k<nGrepPat; k++)
            {
               char *pszPat = glblGrepPat.getString(k);
               int nPatLen = strlen(pszPat);
               int nTmpLen = strlen(pszTmp);
               int nCur = 0, nRel = 0;
               while (mystrhit(pszTmp+nCur, pszPat, cs.usecase, &nRel))
               {
                  if (iMinHitOff < 0 || nRel < iMinHitOff)
                     iMinHitOff = nRel;
                  int iMaxOff = nRel+nPatLen;
                  if (iMaxHitOff < 0 || iMaxOff > iMaxHitOff)
                     iMaxHitOff = iMaxOff;
                  if (nCur+nRel+nPatLen < (int)sizeof(szClAttBuf)-10)
                     memset(&szClAttBuf[nCur+nRel], 'i', nPatLen);
                  nCur += nRel+nPatLen;
                  if (nCur >= nTmpLen-1)
                     break;
               }
            }
            if (iMinHitOff < 0 || cs.joinlines == 0)
               iMinHitOff = 0;
            if (cs.rtrim) {
               // trim line to result range
               int itrim = cs.rtrim-1;
               if (iMaxHitOff + itrim > BINTEXT_RECSIZE)
                   itrim = 0;
               pszTmp[iMaxHitOff+itrim] = '\0';
               szClAttBuf[iMaxHitOff+itrim] = '\0';
            } else {
               // trim joined line to normal wrap width
               int iMaxHitLen = iMaxHitOff - iMinHitOff;
               int iPrintLen  = strlen(pszTmp) - iMinHitOff;
               int iBestLen   = cs.wrapbincol/2-iMargin;
               if (   iPrintLen > 0
                   && iMinHitOff+iPrintLen < BINTEXT_RECSIZE
                   && iPrintLen > iBestLen
                   && iMaxHitLen < iBestLen
                   )
               {
                  iPrintLen = iBestLen;
                  pszTmp[iMinHitOff+iPrintLen] = '\0';
                  szClAttBuf[iMinHitOff+iPrintLen] = '\0';
               }
            }
            if (chain.coldata) {
               // FIX: 163: if prefix, append after that
               if (bPrefixed)
                  chain.addToCurLine(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff, 0);
               else
                  chain.addLine(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff);
            } else {
               printColorText(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff);
            }
         }
 
         // create coloured display of hits of CURRENT line
         if (!cs.justrc && !bskipcur && nMatchCur)
         {
            char *pszTmp = pszBuf;
            bool bPrefixed = 0;

            memset(szClAttBuf, ' ', sizeof(szClAttBuf));
            szClAttBuf[sizeof(szClAttBuf)-1] = '\0';
            if (bGlblGrepLineNum)
               sprintf(szClPreBuf, "   %04u ", nLine);
            else
               sprintf(szClPreBuf, "   ");
            if (nMatchPre) {
               szClPreBuf[1] = '\\';
               szClAttBuf[1] = 'p';
            }

            int iMargin = 1;
            if (!cs.pure && !cs.noind) {
               if (chain.coldata) {
                  // FIX: 163: create new record here
                  chain.addLine(szClPreBuf, szClAttBuf, 0);
                  bPrefixed = 1;
               } else {
                   printColorText(szClPreBuf, szClAttBuf, 0); // w/o LF
               }
               iMargin += strlen(szClPreBuf);
            }

            int iMinHitOff = -1;
            int iMaxHitOff = -1;

            memset(szClAttBuf, ' ', sizeof(szClAttBuf));
            szClAttBuf[sizeof(szClAttBuf)-1] = '\0';
            for (int k=0; k<nGrepPat; k++)
            {
               char *pszPat = glblGrepPat.getString(k);
               int nPatLen = strlen(pszPat);
               int nTmpLen = strlen(pszTmp);
               int nCur = 0, nRel = 0;
               while (mystrhit(pszTmp+nCur, pszPat, cs.usecase, &nRel))
               {
                  if (iMinHitOff < 0 || nRel < iMinHitOff)
                     iMinHitOff = nRel;
                  int iMaxOff = nRel+nPatLen;
                  if (iMaxHitOff < 0 || iMaxOff > iMaxHitOff)
                     iMaxHitOff = iMaxOff;
                  if (nCur+nRel+nPatLen < (int)sizeof(szClAttBuf)-10)
                     memset(&szClAttBuf[nCur+nRel], 'i', nPatLen);
                  nCur += nRel+nPatLen;
                  if (nCur >= nTmpLen-1)
                     break;
               }
            }
            if (iMinHitOff < 0 || cs.joinlines == 0)
               iMinHitOff = 0;
            if (cs.rtrim) {
               // trim line to result range
               int itrim = cs.rtrim-1;
               if (iMaxHitOff + itrim > BINTEXT_RECSIZE)
                   itrim = 0;
               pszTmp[iMaxHitOff+itrim] = '\0';
               szClAttBuf[iMaxHitOff+itrim] = '\0';
            } else {
               // trim current line to normal wrap width
               int iMaxHitLen = iMaxHitOff - iMinHitOff;
               int iPrintLen  = strlen(pszTmp) - iMinHitOff;
               int iBestLen   = cs.wrapbincol/2-iMargin;
               if (   iPrintLen > 0
                   && iMinHitOff+iPrintLen < BINTEXT_RECSIZE
                   && iPrintLen > iBestLen
                   && iMaxHitLen < iBestLen
                   )
               {
                  iPrintLen = iBestLen;
                  pszTmp[iMinHitOff+iPrintLen] = '\0';
                  szClAttBuf[iMinHitOff+iPrintLen] = '\0';
               }
            }
            if (chain.coldata) {
               // FIX: 163: if prefix, append after that
               if (bPrefixed)
                  chain.addToCurLine(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff, 0);
               else
                  chain.addLine(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff);
            } else {
               printColorText(pszTmp+iMinHitOff, szClAttBuf+iMinHitOff);
            }
         }

         // line was listed, do NOT remember
         szClLastLine[0] = '\0';
 
         if (cs.useFirstHitOnly)
            iRC = 1;
      }
      else
      {
         // line was NOT listed
         szClLastLine[0] = '\0';
         // FIX: 1703: ALWAYS take over current line to lastline no matter if hardwrap
         // if (!bHardWrap)
         {
            int nCurLen = strlen(pszBuf);
            int nCopyIndex = 0;
            if (nCurLen > BINTEXT_RECSIZE) {
               nCopyIndex = nCurLen - BINTEXT_RECSIZE;
               nCurLen    = BINTEXT_RECSIZE;
            }
            // mystrcopy guarantees a zero terminator if nCurLen > 0.
            mystrcopy(szClLastLine, pszBuf+nCopyIndex, nCurLen+1);
         }
      }
   }

   return iRC;
}

// - - - sfk internals needed everywhere

FileCloser::FileCloser(Coi *pcoi) {
   pClCoi = pcoi;
}
FileCloser::~FileCloser() {
   if (pClCoi && pClCoi->isFileOpen())
      pClCoi->close();
}

void setAddSnapMeta(uint nmask) { cs.addsnapmeta = nmask; }

int bGlblPassThroughSnap = 0;

mytime_t zipTimeToMainTime(num nZipTime)
{
   mytime_t now = mytime(NULL);
   struct tm *tm = 0;
   tm = mylocaltime(&now);
   tm->tm_isdst = -1;

   tm->tm_year = ((int)(nZipTime >> 25) & 0x7f) + (1980 - 1900);
   tm->tm_mon  = ((int)(nZipTime >> 21) & 0x0f) - 1;
   tm->tm_mday = ((int)(nZipTime >> 16) & 0x1f);

   tm->tm_hour = (int)((unsigned)nZipTime >> 11) & 0x1f;
   tm->tm_min  = (int)((unsigned)nZipTime >>  5) & 0x3f;
   tm->tm_sec  = (int)((unsigned)nZipTime <<  1) & 0x3e;

   // rebuild main time
   mytime_t nTime = mymktime(tm); // sfk1933

   // check for overflows
   #ifndef S_TIME_T_MAX
    #define S_TIME_T_MAX ((mytime_t)0x7fffffffUL)
   #endif

   #ifndef U_TIME_T_MAX
    #define U_TIME_T_MAX ((mytime_t)0xffffffffUL)
   #endif

   #define DOSTIME_2038_01_18 ((uint)0x74320000L)
   if ((nZipTime >= DOSTIME_2038_01_18) && (nTime < (mytime_t)0x70000000L))
      nTime = U_TIME_T_MAX;

   if (nTime < (mytime_t)0L) // sfk1933
      nTime = S_TIME_T_MAX;

   return nTime;
}

bool equalFileContent(char *pszFile1, char *pszFile2, uchar *psrcmd5, uchar *pdstmd5)
{
   if (psrcmd5) memset(psrcmd5, 0, 16);
   if (pdstmd5) memset(pdstmd5, 0, 16);
   uchar absum1[20];
   uchar absum2[20];
   if (getFileMD5(pszFile1, absum1)) return false;
   if (psrcmd5) memcpy(psrcmd5, absum1, 16);
   if (getFileMD5(pszFile2, absum2)) return false;
   if (pdstmd5) memcpy(pdstmd5, absum2, 16);
   if (!memcmp(absum1, absum2, 16))
      return 1; // equal
   return 0;
}

uint getLong(uchar ab[], uint noffs) {
   return  (((uint)ab[noffs+3])<<24)
          |(((uint)ab[noffs+2])<<16)
          |(((uint)ab[noffs+1])<< 8)
          |(((uint)ab[noffs+0])<< 0);
}

uint getShort(uchar ab[], uint noffs) {
   return  (((uint)ab[noffs+1])<< 8)
          |(((uint)ab[noffs+0])<< 0);
}

bool matchesCurrentRoot(char *pszDir)
{
   if (!glblFileSet.hasRoot(0)) return false;
   char *pszRoot = glblFileSet.getCurrentRoot();
   if (!pszRoot) { perr("internal 812064\n"); return false; }
   bool brc = equalFileName(pszDir, pszRoot);
   // printf("%d = mcr(%s)\n", brc, pszDir);
   return brc;
}



// used with old execReplaceFix if srclen==replen
int diffDump(uchar *p1, uchar *p2, num nlen, num nListOffset, int iHiOff, int iHiLen)
{__
   num nSubOff = 0, iCurOff = 0;
   num nRemain = nlen;

   char cframe = 'h';

   mclear(szAttrBuf);

   while (nRemain > 0)
   {
      num nBlockLen = nRemain;
      int *appos = 0;

      if (bGlblHexDumpWide)
      {
      strcpy(szLineBuf,
      //  1                                    38               55                73                                   110
      //">00112233 44556677 00112233 44556677< 0123456789ABCDEF 0123456789ABCDEF >00112233 44556677 00112233 44556677< 0123456789ABCDEF");
        ">                                   <                                   >                                   <                 ");
      sprintf(szAttrBuf,
        "%c                                   %c                                   %c                                   %c                 ",
        cframe,cframe,cframe,cframe);
      //                          0  1   2   3   4
      static int aPosWide[] = {  1, 38, 55, 73, 110 };
      appos = aPosWide;
      if (nBlockLen > 16) nBlockLen = 16;
      }
      else
      {
      strcpy(szLineBuf,
      //  1                  20       29        39                 58
      //">00112233 44556677< 01234567 01234567 >00112233 44556677< 0123456789ABCDEF");
        ">                 <                   >                 <                 ");
      sprintf(szAttrBuf,
        "%c                 %c                   %c                 %c                 ",
        cframe,cframe,cframe,cframe);
      //                          0  1   2   3   4
      static int aPosBase[] = {  1, 20, 29, 39, 58 };
      appos = aPosBase;
      if (nBlockLen > 8) nBlockLen = 8;
      }

      // create offset info
      char *pszHexOff = numtohex(nListOffset+nSubOff, 8);
      strcpy(szLineBuf+appos[4], pszHexOff); // ipos
      int iofflen = strlen(pszHexOff);
      memset(szAttrBuf+appos[4], cframe, iofflen);

      for (num i=0; i<nBlockLen; i++)
      {
         iCurOff = nSubOff+i;

         // create hex info
         uchar uc1 = p1[nSubOff+i];
         uchar uc2 = p2[nSubOff+i];
         int iof1 = appos[0] + i * 2 + ((i>=4)?1:0) + ((i>=8)?1:0) + ((i>=12)?1:0);
         int iof2 = appos[3] + i * 2 + ((i>=4)?1:0) + ((i>=8)?1:0) + ((i>=12)?1:0);
         sprintf(szLineBuf+iof1, "%02X", uc1);
         sprintf(szLineBuf+iof2, "%02X", uc2);
         szLineBuf[iof1+2] = ' ';
         szLineBuf[iof2+2] = ' ';

         // create printable info
         int iof3 = appos[1] + i;  // ipos
         int iof4 = appos[2] + i;  // ipos

         if(isprint(uc1)) {
            szLineBuf[iof3] = (char)uc1;
            // szAttrBuf[iof3] = 'i';
         } else
            szLineBuf[iof3] = '.';

         if(isprint(uc2)) {
            szLineBuf[iof4] = (char)uc2;
            // szAttrBuf[iof4] = 'i';
         } else
            szLineBuf[iof4] = '.';

         // highlight different bytes
         if (   (iHiOff >= 0 && iCurOff >= iHiOff && iCurOff < iHiOff+iHiLen)
             || (uc1 != uc2)
            )
         {
            szAttrBuf[iof1+0] = 'a';
            szAttrBuf[iof1+1] = 'a';
            szAttrBuf[iof2+0] = 'a';
            szAttrBuf[iof2+1] = 'a';
            szAttrBuf[iof3]   = 'a';
            szAttrBuf[iof4]   = 'a';
         }
      }
      szLineBuf[appos[1]-2] = '<'; // ipos
      szLineBuf[appos[4]-2] = '<'; // ipos

      // if in binfind mode, show just left part, appending offset
      if (cs.repDumpHalve)
      {
         int iofm = appos[2];
         int iofo = appos[4];
         strcpy(szLineBuf+iofm, szLineBuf+iofo);
         strcpy(szAttrBuf+iofm, szAttrBuf+iofo);
      }

      if (chain.coldata) {
         chain.addLine(szLineBuf, szAttrBuf);
      } else {
         printColorText(szLineBuf, szAttrBuf);
      }

      nSubOff += nBlockLen;
      nRemain -= nBlockLen;
   }
   return 0;
}

int execHexdump(Coi *pcoi, uchar *pBuf, uint nBufSize, int iHighOff, int iHighLen, FILE *foutopt, num nListOffset)
{__
   FILE *fout = stdout;
 
   if (foutopt)
         fout = foutopt;

   char cframe = 'h';

   num ntotal = 0;

   uchar *pBufCur = pBuf;
   int nBufRem = (int)nBufSize;

   num nHexDumpOff = nGlblHexDumpOff;

   if (nHexDumpOff < 0) {
      if (!pcoi) return 9;
      num nInFileSize = pcoi->getSize();
      nHexDumpOff = nInFileSize + nHexDumpOff;
      if (nHexDumpOff < 0)
         nHexDumpOff = 0;
   }

   if (pBuf)
   {
      if (nHexDumpOff >= nBufSize)
         return 5;
      pBufCur += nHexDumpOff;
      nBufRem -= nHexDumpOff;
      if (nHexDumpOff)
         ntotal = nHexDumpOff;
   }
   else
   {
      // sfk1840: automatic set of leattr
      if (cs.leauto) {
         if (!pcoi->isBinaryFile())
            cs.leattr = 'e';
      }

      if (pcoi->open("rb")) {
         // non-fatal, continue with other files
         return 1+pwarn("%s : cannot read - skipping\n", pcoi->name());
      }

      const char *pszPrefix = (nGlblHexDumpForm>=5) ? "    ":"";

      if (!cs.nonames) {
         if (chain.coldata) {
            sprintf(szLineBuf2, "%s:file %s", pszPrefix, pcoi->name());
            // note: +view scans extended end of attribute line
            //       to identify 'f'ile header lines, therefore +2:
            setattr(szAttrBuf2, 'f', strlen(szLineBuf2)+2, MAX_LINE_LEN);
            chain.addLine(szLineBuf2, szAttrBuf2);
         } else {
            setTextColor(nGlblFileColor);
            if (cs.rawfilename)
               fprintf(fout, "%s :\n", pcoi->name());
            else
               fprintf(fout, "%s:file %s\n", pszPrefix, pcoi->name());
            setTextColor(-1);
         }
      }

      if (nHexDumpOff) {
         if (pcoi->seek(nHexDumpOff, SEEK_SET)) {
            pcoi->close();
            return 1+pwarn("%s : cannot seek - skipping\n", pcoi->name());
         }
         ntotal = nHexDumpOff;
      }
   }

   int lOutLen2=0, lIndex=0, lIndex2=0;
   int lRelPos=0;
   uchar *pTmp = 0;
   uchar ucTmp;
   uchar abBlockBuf[2500]; // sfk1952 up to 2000 input bytes

   int nbpl  = bGlblHexDumpWide ?  32 : 16; // bytes per line
   int itext = bGlblHexDumpWide ?  75 : 39; // text begin
   int ioffs = bGlblHexDumpWide ? 108 : 56; // offset begin
   int ieol  = ioffs + 20;
 
   if (nGlblHexDumpForm && cs.bytesperline)
       nbpl = cs.bytesperline;

   num  nTotalMax = 0;
   if (nGlblHexDumpLen > 0)
        nTotalMax = nHexDumpOff + nGlblHexDumpLen;

   if (cs.verbose && nGlblHexDumpLen) {
      fprintf(fout, "dump from %s ",numtoa(nHexDumpOff));
      fprintf(fout, "to %s ",numtoa(nTotalMax));
      fprintf(fout, "a total of %s bytes\n",numtoa(nGlblHexDumpLen));
   }

   bool bNoBlockTrail = (cs.dumptrail & 1) ? 1 : 0;
   bool bNoLineTrail  = (cs.dumptrail & 2) ? 1 : 0;

   uint uiCharPos = 0;

   while (!userInterrupt()) // sfk1914
   {
      int nread = 0;
      if (pBuf) {
         if (nBufRem <= 0)
            break;
         if (nbpl < nBufRem) nread = nbpl;
         else                nread = nBufRem;
         memcpy(abBlockBuf, pBufCur, nread);
         pBufCur += nread;
         nBufRem -= nread;
      } else {
         nread = pcoi->read(abBlockBuf, nbpl);
      }
      if (nread <= 0) break;
      pTmp = abBlockBuf;

      int lOutLen = nread;
 
      // dump a full or partial output line?
      if (nTotalMax > 0) {
         num nTotalRemain = nTotalMax - ntotal;
         if (lOutLen > nTotalRemain)
            lOutLen = nTotalRemain;
         if (lOutLen <= 0)
            break;
      }

      szLineBuf[0] = '\0';
      bool bshort  = 0;
      bool bEOD    = 0;
 
      // last record?
      if (nTotalMax > 0)
         if (ntotal + nread >= nTotalMax)
            bEOD = 1;

      switch (nGlblHexDumpForm)
      {
         case 0:
            break; // fall through

         case 1: { // pure
            for (int i=0; i<lOutLen; i++)
               mystrcatf(szLineBuf,MAX_LINE_LEN,"%02X",pTmp[i]);
            if (!cs.nolf)
               strcat(szLineBuf,"\n");
            bshort = 1;
            break;
         }

         case 2: { // source, hex
            for (int i=0; i<lOutLen; i++)
               mystrcatf(szLineBuf,MAX_LINE_LEN,"0x%02X,",pTmp[i]);
            int iLen = strlen(szLineBuf);
            if (bNoLineTrail || (bEOD && bNoBlockTrail))
               if (iLen > 0 && szLineBuf[iLen-1] == ',')
                  szLineBuf[iLen-1] = '\0';
            if (!cs.nolf)
               strcat(szLineBuf,"\n");
            bshort = 1;
            break;
         }

         case 3: { // source, dec
            for (int i=0; i<lOutLen; i++)
               mystrcatf(szLineBuf,MAX_LINE_LEN,"%u,",(uint)pTmp[i]);
            int iLen = strlen(szLineBuf);
            if (bNoLineTrail || (bEOD && bNoBlockTrail))
               if (iLen > 0 && szLineBuf[iLen-1] == ',')
                  szLineBuf[iLen-1] = '\0';
            if (!cs.nolf)
               strcat(szLineBuf,"\n");
            bshort = 1;
            break;
         }

         case 4: { // flat, filtering control characters, skipping binary
            for (int i=0; i<lOutLen; i++) {
               uchar c = pTmp[i];
               if (!c) {
                  int nRemain = lOutLen - i;
                  mystrcatf(szLineBuf,MAX_LINE_LEN," [binary, skipping %u bytes]\n", nRemain);
                  break;
               }
               else
               if (c < 0x20 && (c != '\r' && c != '\n' && c != '\t'))
                  continue; // skip control characters except LF, TAB
               else
                  mystrcatf(szLineBuf,MAX_LINE_LEN,"%c",(char)c);
            }
            bshort = 1;
            break;
         }

         case 5: // forum
         case 6: // minimal
         {
         // strcpy(szLineBuf, "    -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- # ---------------- ----");
            strcpy(szLineBuf, "                                                    #                      ");
                           //  01234567890123456789012345678901234567890123456789012345678901234567890123456789
                           //            1         2         3          4        5         6         7
            for (int i=0; i<lOutLen; i++) {
               uchar c = pTmp[i];
               mystrplot(szLineBuf+4+i*3, 2, "%02X", c);
               if (   (nGlblHexDumpForm==5 && isprint(c)!=0)
                   || (nGlblHexDumpForm==6 && isalnum(c)!=0)
                  )
                  szLineBuf[54+i] = c;
               else
                  szLineBuf[54+i] = '.';
            }
            sprintf(szLineBuf+71, "%04X\n", (int)ntotal);
            bshort = 1;
            break;
         };
      }

      if (bshort)
      {
         // dump short form created above
         if (chain.coldata) {
            removeCRLF(szLineBuf); // FIX 169
            if (cs.nolf && chain.colbinary) { // sfk1882
               if (chain.addBinary((uchar*)szLineBuf, strlen(szLineBuf)))
                  return 9;
            } else {
               chain.addLine(szLineBuf, str(""), 0);
            }
         } else {
            fprintf(fout,"%s",szLineBuf);
         }
      }
      else
      {
         // dump full hex format with offset and ascii
         memset(szLineBuf, ' ', ieol);
         memset(szAttrBuf, ' ', ieol);
         szLineBuf[ieol] = '\0';
         szAttrBuf[ieol] = '\0';
 
         szLineBuf[1] = '>';
         szAttrBuf[1] = cframe;

         char *pszHexOff = numtohex(ntotal+nListOffset, 8);
         strcpy(&szLineBuf[ioffs], pszHexOff);
         // adds zero terminator after offset info!
         int iofflen = strlen(pszHexOff);
         memset(szAttrBuf+ioffs, cframe, iofflen);
         int ieol = strlen(szLineBuf);
         szAttrBuf[ieol] = '\0';
 
         lOutLen2 = lOutLen;
 
         for(lIndex = 2, lIndex2 = itext, lRelPos = 0;
             lOutLen2;
             lOutLen2--, lIndex += 2, lIndex2++
            )
         {
            ucTmp = *pTmp++;
 
            sprintf(szLineBuf + lIndex, "%02X ", (unsigned short)ucTmp);
 
            // optional: highlight CR and LF characters
            if (cs.leattr)
               if (ucTmp == '\r' || ucTmp == '\n') {
                  szAttrBuf[lIndex+0] = cs.leattr;
                  szAttrBuf[lIndex+1] = cs.leattr;
                  szAttrBuf[lIndex2 ] = cs.leattr;
               }

            if(isprint(ucTmp))
               szAttrBuf[lIndex2] = ' ';  // mark printable text
            else
               ucTmp = '.'; // nonprintable char

            // optional: highlight given area
            if (iHighOff != -1) {
               uiCharPos = ntotal + lRelPos;
               if (uiCharPos >= iHighOff && uiCharPos < iHighOff+iHighLen) {
                  szAttrBuf[lIndex+0] = 'i';
                  szAttrBuf[lIndex+1] = 'i';
                  szAttrBuf[lIndex2 ] = 'i';
               }
            }

            szLineBuf[lIndex2] = ucTmp;

            if (!(++lRelPos & 3))     // extra blank after 4 bytes
            {  lIndex++; szLineBuf[lIndex+2] = ' '; }
         }
 
         if (!(lRelPos & 3)) lIndex--;
 
         szLineBuf[lIndex  ]   = '<';
         szLineBuf[lIndex+1]   = ' ';
         szAttrBuf[lIndex ]    = cframe;
 
         if (chain.coldata) {
            chain.addLine(szLineBuf, szAttrBuf);
         }
         else if (foutopt) {
            fprintf(fout, "%s\n", szLineBuf);
         }
         else {
            printColorText(szLineBuf, szAttrBuf);
         }
      }

      ntotal += (num)nread;

      if (bEOD)
         break;
   }

   if (foutopt)
      fflush(fout);

   if (!pBuf)
      pcoi->close();

   return 0;
}

int termHexdump(uchar *pBuf, uint nBufSize, int bwide)
{__
   char cframe = 'h';

   num ntotal = 0;

   uchar *pBufCur = pBuf;
   int nBufRem = (int)nBufSize;

   int lOutLen2=0, lIndex=0, lIndex2=0;
   int lRelPos=0;
   uchar *pTmp = 0;
   uchar ucTmp;
   uchar abBlockBuf[1000];

   if (bwide != -1) bGlblHexDumpWide = bwide;

   int nbpl  = bGlblHexDumpWide ?  32 : 16; // bytes per line
   int itext = bGlblHexDumpWide ?  75 : 39; // text begin
   int ioffs = bGlblHexDumpWide ? 108 : 56; // offset begin
   int ieol  = ioffs + 20;
 
   uint uiCharPos = 0;

   while (1)
   {
      int nread = 0;
      if (nBufRem <= 0)
         break;
      if (nbpl < nBufRem) nread = nbpl;
      else                nread = nBufRem;
      memcpy(abBlockBuf, pBufCur, nread);
      pBufCur += nread;
      nBufRem -= nread;
      if (nread <= 0) break;
      pTmp = abBlockBuf;

      int lOutLen = nread;
 
      szLineBuf[0] = '\0';
      bool bshort  = 0;
      bool bEOD    = 0;
 
      strcpy(szLineBuf, "                                                    #                      ");
      for (int i=0; i<lOutLen; i++) {
         uchar c = pTmp[i];
         mystrplot(szLineBuf+4+i*3, 2, "%02X", c);
         if (   (nGlblHexDumpForm==5 && isprint(c)!=0)
             || (nGlblHexDumpForm==6 && isalnum(c)!=0)
            )
            szLineBuf[54+i] = c;
         else
            szLineBuf[54+i] = '.';
      }
      sprintf(szLineBuf+71, "%04X\n", (int)ntotal);

      {
         // dump full hex format with offset and ascii
         memset(szLineBuf, ' ', ieol);
         memset(szAttrBuf, ' ', ieol);
         szLineBuf[ieol] = '\0';
         szAttrBuf[ieol] = '\0';
 
         szLineBuf[1] = '>';
         szAttrBuf[1] = cframe;

         char *pszHexOff = numtohex(ntotal, 8);
         strcpy(&szLineBuf[ioffs], pszHexOff);
         // adds zero terminator after offset info!
         int iofflen = strlen(pszHexOff);
         memset(szAttrBuf+ioffs, cframe, iofflen);
         int ieol = strlen(szLineBuf);
         szAttrBuf[ieol] = '\0';
 
         lOutLen2 = lOutLen;
 
         for(lIndex = 2, lIndex2 = itext, lRelPos = 0;
             lOutLen2;
             lOutLen2--, lIndex += 2, lIndex2++
            )
         {
            ucTmp = *pTmp++;
 
            sprintf(szLineBuf + lIndex, "%02X ", (unsigned short)ucTmp);
 
            if (ucTmp == '\r' || ucTmp == '\n') {
               szAttrBuf[lIndex+0] = 'e';
               szAttrBuf[lIndex+1] = 'e';
               szAttrBuf[lIndex2 ] = 'e';
            }

            if(isprint(ucTmp))
               szAttrBuf[lIndex2] = ' ';  // mark printable text
            else
               ucTmp = '.'; // nonprintable char

            szLineBuf[lIndex2] = ucTmp;

            if (!(++lRelPos & 3))     // extra blank after 4 bytes
            {  lIndex++; szLineBuf[lIndex+2] = ' '; }
         }
 
         if (!(lRelPos & 3)) lIndex--;
 
         szLineBuf[lIndex  ]   = '<';
         szLineBuf[lIndex+1]   = ' ';
         szAttrBuf[lIndex ]    = cframe;
 
         printColorText(szLineBuf, szAttrBuf);
      }

      ntotal += (num)nread;

      if (bEOD)
         break;
   }

   return 0;
}

// original src : thedir
// have now     : thedir/subdir/thefile.txt
// need relative:        subdir/thefile.txt
char *relName(char *pszRoot, char *pszAbs)
{
   bool bisurl = 0;
   // FIX: 163R5: crash on sfk sel ... +copy due to missing null ptr check
   if (!pszRoot || !strlen(pszRoot))
       return pszAbs;
   #ifdef VFILEBASE
   bisurl =    !strncmp(pszRoot, "ftp://", 6)
            || !strncmp(pszRoot, "http://", 7)
            || !strncmp(pszRoot, "https://", 7);
   // TODO: for now, block relativazation of all http: urls
   if (!strncmp(pszRoot, "http://", 7))
      return pszAbs;
   if (!strncmp(pszRoot, "https://", 8))
      return pszAbs;
   #endif // VFILEBASE
   int nsrclen = strlen(pszRoot);
   if (!strcmp(pszRoot, "."))
       return pszAbs;
   if (!strcmp(pszRoot, pszAbs))
       return pszAbs + nsrclen;
   // have to compare copysrc WITH path separator char!
   if (endsWithPathChar(pszRoot, bisurl))
       nsrclen--;
   // first check name part
   if (strncmp(pszRoot, pszAbs, nsrclen)) {
       pwarn("%s not relative to %s (1)\n",pszAbs,pszRoot);
       return pszAbs; // no match
   }
   // then also path char
   #ifdef _WIN32
   // IF dir is not ending with ':'
   if (endsWithColon(pszRoot))
       return pszAbs + nsrclen;
   #endif
   if (bisurl && pszAbs[nsrclen] == '/')
     { }
   else
   if (pszAbs[nsrclen] != glblPathChar) {
       pwarn("%s not relative to \"%s\" (2) %c\n",pszAbs,pszRoot,pszAbs[nsrclen]);
       return pszAbs; // no match
   }
   char *pszRel = pszAbs + nsrclen + 1; // past separator
   // printf("REL %s => %s\n", pszAbs, pszRel);
   return pszRel;
}

// c:the\foo\bar.txt -> bar.txt
// c:bar.txt -> bar.txt
char *relativeFilename(char *pszPath)
{
   char *pszRel = strrchr(pszPath, glblPathChar);
   if (pszRel) return pszRel+1;

   #ifdef _WIN32
   pszRel = strrchr(pszPath, ':');
   if (pszRel) return pszRel+1;
   #endif

   return pszPath;
}

// returns NULL in case of error.
// returns same name if root doesn't match.
char *rootRelativeName(char *pszFileName, char *pszOptRoot)
{
   if (pszOptRoot)
      return relName(pszOptRoot, pszFileName);

   if (!glblFileSet.hasRoot(0))
      return pszFileName;

   char *pszRoot = glblFileSet.getCurrentRoot();
   if (!pszRoot) { perr("internal 812063\n"); return 0; }

   return relName(pszRoot, pszFileName);
}

int checkArgCnt(int argc, int lMinCnt) {
   if (argc < lMinCnt)
      return 9+perr("missing arguments. type \"sfk\" without parms for help.\n");
   return 0;
}

// uses szLineBuf2, and szLineBuf indirectly, for result.
// RC: number of hits
int listPathAny(char *pszCmd, bool bSilent)
{
   int nhits = 0;

   #ifdef _WIN32

   // check PATHEXT=.COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH
   char *pszExt = getenv("PATHEXT");
   if (!pszExt) {
      char *psz = findPathLocation(pszCmd, 0);
      if (psz) { nhits++; if (!bSilent) chain.print("%s\n", psz); }
   } else {
      pszExt = strdup(pszExt);
      char *psz1 = pszExt;
      bool beod = 0;
      while (*psz1 && !beod) {
         // recombine base string with any extension like .exe, .cmd
         char *psz2 = psz1;
         while (*psz2 && *psz2 != ';')
            { *psz2 = sfktolower(*psz2); psz2++; }
         if (*psz2) *psz2++ = '\0';
         else beod = 1;
         // check is such a file is in path. if so, list it.
         sprintf(szLineBuf2, "%s%s", pszCmd, psz1);
         char *psz = findPathLocation(szLineBuf2, 0);
         if (psz) { nhits++; if (!bSilent) chain.print("%s\n", psz); }
         psz1 = psz2;
      }
      delete [] pszExt;
   }

   #else

   char *psz = findPathLocation(pszCmd, 0);
   if (psz) { nhits++; if (!bSilent) chain.print("%s\n", psz); }

   #endif

   return nhits;
}

char *getWritePath(int &rerr)
{
   char *pszSFKPath=getenv("SFK_PATH");
   if (!pszSFKPath) return 0;

   char *pszPath = getenv("PATH");
   if (!pszPath) return 0;

   char *psz1 = pszPath;
   while (*psz1)
   {
      char *psz2 = psz1;
      #ifdef _WIN32
      while (*psz2 && (*psz2 != ';'))
         psz2++;
      #else
      while (*psz2 && (*psz2 != ':'))
         psz2++;
      #endif

      // isolate single directory from path.
      int nLen = psz2-psz1;
      strncpy(szLineBuf, psz1, nLen);
      szLineBuf[nLen] = '\0';

      // now holding single dir in szLineBuf.
      stripTrailingBackSlashes(szLineBuf);
      if (!strcmp(pszSFKPath, szLineBuf))
         return pszSFKPath;

      // step to next subpath
      if (*psz2)
         psz2++;
      psz1 = psz2;
   }

   perr("SFK_PATH folder '%s' not found in PATH", pszSFKPath);
   pinf("set the SFK_PATH environment variable only\n");
   pinf("to a folder which is present in the PATH.\n");
   rerr = 9;

   return 0;
}

// uses szLineBuf, also for result!
char *findPathLocation(cchar *pszCmd, bool bExcludeWorkDir)
{
   #ifdef _WIN32
   if (!bExcludeWorkDir) {
      // win only: check current dir first (implicite path inclusion)
      szLineBuf[0] = '\0';
      if (getcwd(szLineBuf,MAX_LINE_LEN-10)) { }
      strcat(szLineBuf,glblPathStr);
      strcat(szLineBuf,pszCmd);
      if (fileExists(szLineBuf)) {
         if (cs.debug)
            printf("hit: %s [cwd]\n", szLineBuf);
         return szLineBuf;
      }
   }
   #endif

   char *pszPath = getenv("PATH");
   if (!pszPath)
   {
      #ifndef DVIEW
      perr("no PATH variable found.\n");
      #endif
      return 0;
   }
   char *psz1 = pszPath;
   while (*psz1)
   {
      char *psz2 = psz1;
      #ifdef _WIN32
      while (*psz2 && (*psz2 != ';'))
         psz2++;
      #else
      while (*psz2 && (*psz2 != ':'))
         psz2++;
      #endif
      // isolate single directory from path.
      int nLen = psz2-psz1;
      strncpy(szLineBuf, psz1, nLen);
      szLineBuf[nLen] = '\0';
      // now holding single dir in szLineBuf.
      if (!strcmp(szLineBuf, ".") && bExcludeWorkDir) {
         if (cs.debug)
            printf("skip: %s\n", szLineBuf);
      } else {
         if (cs.debug)
            printf("probe: %s\n", szLineBuf);
         stripTrailingBackSlashes(szLineBuf);
         strcat(szLineBuf, glblPathStr);
         strcat(szLineBuf, pszCmd);
         if (fileExists(szLineBuf)) {
            if (cs.debug)
               printf("hit: %s\n", szLineBuf);
            return szLineBuf;
         }
      }
      // step to next subpath
      if (*psz2)
         psz2++;
      psz1 = psz2;
   }
   return 0;
}

char *localPath(char *pAbsFile)
{
   static char szPath[SFK_MAX_PATH+10];
   strcopy(szPath, pAbsFile);
   if (cs.test == 0) { // sfk1840 ftp -test support
      for (char *p=szPath; *p; p++) {
         if (*p == glblWrongPChar)
             *p = glblPathChar;
      }
   }
   return szPath;
}

char *remoteStandardPath(char *pAbsFile)
{
   static char szPath[1024];
   strcopy(szPath, pAbsFile);
   for (char *p=szPath; *p; p++) {
      if (*p == '\\')
          *p = '/';
   }
   return szPath;
}

// sfk184: complete rework of traversal check
//         to detect /..\ and C:\foo.txt
bool isPathTraversal(char *pszFileIn, bool bDeep)
{
   if (strlen(pszFileIn) > SFK_MAX_PATH) return 1;

   char szBuf[SFK_MAX_PATH+100];

   strcopy(szBuf, pszFileIn);
   char *pszFile = szBuf;

   // for normalized check using only '/'
   for (int i=0; szBuf[i]; i++)
      if (szBuf[i]=='\\')
         szBuf[i]='/';

   if (!strlen(pszFile)) return 1;

   int ilen = strlen(pszFile);

   if (pszFile[0] == '/') return 1;
   if (!strcmp(pszFile, ".")) return 1;
   if (!strcmp(pszFile, "./")) return 1;
   if (!strcmp(pszFile, "/")) return 1;
   if (!strcmp(pszFile, ".."))  return 1;
   if (!strncmp(pszFile, "../", 3)) return 1;
   if (strstr(pszFile, "/../")) return 1;
   if (ilen >= 3 && !strcmp(pszFile + ilen - 3, "/.."))
      return 1;

   #ifdef _WIN32
   // sfk184: block ALL C:\... access
   if (isalpha(pszFile[0])!=0 && pszFile[1]==':') return 1; // sfk1840
   #endif

   if (!bDeep)
   {
      if (strstr(pszFile, "/")) return 1;
   }

   return 0;
}

#ifdef SFKINT
void traversalTest()
{
   bool bdeep = 1;

   printf("expect 0:\n");
   printf("%d\n", isPathTraversal("mydir/foo/bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir/foo ../bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir/foo.../bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir/foo...\\bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir../bar.txt",bdeep));
   printf("%d\n", isPathTraversal("...",bdeep));
   printf("%d\n", isPathTraversal(".cfg",bdeep));
   printf("%d\n", isPathTraversal("foo..\\test.txt",bdeep));

   printf("expect 1:\n");
   printf("%d\n", isPathTraversal(".",bdeep));
   printf("%d\n", isPathTraversal("mydir/../bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir/..\\bar.txt",bdeep));
   printf("%d\n", isPathTraversal("mydir/..",bdeep));
   printf("%d\n", isPathTraversal("mydir\\..",bdeep));
   printf("%d\n", isPathTraversal("..",bdeep));
   printf("%d\n", isPathTraversal("..\\",bdeep));
   printf("%d\n", isPathTraversal("C:\\foo.txt",bdeep));
   printf("%d\n", isPathTraversal("C:/foo.txt",bdeep));
   printf("%d\n", isPathTraversal("C:\\foo\\bar.txt",bdeep));
   printf("%d\n", isPathTraversal("C:/foo/bar.txt",bdeep));
}
#endif

// flags: bit 0: convert also \, to ,
// flags: bit 1: ignore cs.spat, assume -spat and -strict
int copyFormStr(char *pszDst, int nMaxDst, char *pszSrc, int nSrcLen, uint nflags)
{
   // printf("copyFormStr \"%.*s\" %d spat=%d\n",(int)nSrcLen,pszSrc,nflags,cs.spat);

   int ispat = cs.spat;

   if (nflags & 2)
       ispat = 1;

   char *pszin = pszSrc;
   int iout = 0;
   while (*pszin && (nSrcLen > 0))
   {
      if (iout >= nMaxDst-10)
         return 9+perr("format string too long: \"%s\"\n", pszSrc);

      if (!ispat)
         { } // skip all following interpretations
      else
      if (nSrcLen>=4 && !strncmp(pszin, "\\x", 2)) {
         // \xnn - any character with hex code nn
         pszin+=2; nSrcLen-=2; // skip \x
         int nhex = getTwoDigitHex(pszin);
         if (nhex <= 0) return 9+perr("illegal value for \\xnn in format string. use 01 to FF, e.g. \\x09\n");
         pszin+=2; nSrcLen-=2; // skip nn
         pszDst[iout++] = (char)nhex;
         continue;
      }
      else
      if (ispat==2 && nSrcLen>=5 && !strncmp(pszin, "\\d", 2)) {
         // \dnnn - any character with decimal code nnn
         pszin+=2; nSrcLen-=2; // skip \d
         int ndec = getThreeDigitDec(pszin);
         if (ndec <= 0) return 9+perr("illegal value for \\dnnn in format string. use 001 to 255, e.g. \\d065\n");
         pszin+=3; nSrcLen-=3; // skip nnn
         pszDst[iout++] = (char)ndec;
         continue;
      }
      else
      if (nSrcLen>=2) {
         if (!strncmp(pszin, "\\\\", 2))
            { pszDst[iout++] = '\\'; pszin+=2; nSrcLen-=2; continue; }
         else
         if (!strncmp(pszin, "\\t", 2))
            { pszDst[iout++] = '\t'; pszin+=2; nSrcLen-=2; continue; }
         else
         if (!strncmp(pszin, "\\q", 2))
            { pszDst[iout++] = '"'; pszin+=2; nSrcLen-=2; continue; }
         else
         if (!strncmp(pszin, "\\n", 2))
            { pszDst[iout++] = '\n'; pszin+=2; nSrcLen-=2; continue; }
         else
         if (!strncmp(pszin, "\\r", 2))
            { pszDst[iout++] = '\r'; pszin+=2; nSrcLen-=2; continue; }
         else
         if ((nflags & 1) && !strncmp(pszin, "\\,", 2))
            { pszDst[iout++] = ','; pszin+=2; nSrcLen-=2; continue; }
      }
      if ((nflags & 2) != 0 && *pszin == '\\')
         return 9+perr("invalid slash pattern: %s\n", pszin);
      pszDst[iout++] = *pszin++;
      nSrcLen--;
   }
   pszDst[iout] = '\0';
   // printf("-> \"%s\"\n",pszDst);
   return 0;
}

int decodeHex(char *pszSrc, int iSrcLen, char *pszDst, int iMaxDest, char cPrefix)
{
   char  ahex[10];
   uchar abin[10];
   char  *psz=pszSrc;
   char  *pszMax=psz+iSrcLen;

   char  *pDstCur=pszDst;
   char  *pDstMax=pszDst+(iMaxDest-10);

   int  istatedef=1;
   if (cPrefix==0)
        istatedef=2;
   int  istate=istatedef;

   for (;psz<pszMax;psz++)
   {
      if (pDstCur >= pDstMax) {
         if (!cs.quiet)
            perr("buffer overflow on hex decode: %.40s\n", pszSrc);
         return -2;
      }

      // with pure hex, skip all whitespace
      if (cPrefix==0 && isspace(*psz)!=0)
         continue;

      // with prefixed hex, skip line ends,
      // as they must be encoded like _0d_0a
      if (cPrefix!=0 && (*psz=='\r' || *psz=='\n'))
         continue;

      switch (istate)
      {
         case 1: // search prefix
            if (*psz!=cPrefix) {
               *pDstCur++ = *psz;
               continue;
            }
            istate=2;
            continue;

         case 2: // collect 1st hex digit
            ahex[0] = *psz;
            istate=3;
            continue;

         case 3: // collect 2nd hex digit
            ahex[1] = *psz;
            ahex[2] = '\0';
            if (hexToBin(ahex,abin,1)) {
               if (!cs.quiet)
                  perr("invalid hex code: \"%s\" (%02x %02x)\n", ahex, ahex[0], ahex[1]);
               return -1;
            }
            *pDstCur++ = (char)abin[0];
            istate=istatedef;
            continue;
      }
   }

   *pDstCur='\0';

   return (int)(pDstCur-pszDst);
}

int encodeHex(uchar *pszSrc, int iSrcLen, char *pszDst, int iMaxDest, char cPrefix, char *abToEncode)
{
   char  ahex[10];
   uchar abin[10];

   int   iSrcCur=0;
   char  *pDstCur=pszDst;
   char  *pDstMax=pszDst+(iMaxDest-10);

   for (;iSrcCur<iSrcLen;iSrcCur++)
   {
      if (pDstCur >= pDstMax) {
         if (!cs.quiet)
            perr("buffer overflow on %chex encode: %.40s\n", cPrefix?cPrefix:' ', pszSrc);
         return -2;
      }

      uchar uc = pszSrc[iSrcCur];

      if (abToEncode != 0 && abToEncode[uc] == 0) {
         *pDstCur++ = (char)uc;
         continue;
      }

      if (cPrefix!='\0') {
         sprintf(pDstCur,"%c%02X",cPrefix,uc);
         pDstCur += 3;
      } else {
         sprintf(pDstCur,"%02X",uc);
         pDstCur += 2;
      }
   }

   *pDstCur='\0';

   return (int)(pDstCur-pszDst);
}

// base64 en/decoding

// also used by makepw w/o last 2 chars
static const char aenc64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
   "0123456789+/";

void encodeSub64(uchar in[3], uchar out[4], int nlen)
{
   out[0] = aenc64[in[0] >> 2];
   out[1] = aenc64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
   out[2] = (uchar)(nlen > 1 ? aenc64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] : '=');
   out[3] = (uchar)(nlen > 2 ? aenc64[in[2] & 0x3f ] : '=');
   #if 0
   printf("EIN : %u %u %u\n",in[0],in[1],in[2]);
   printf("EOUT: %u %u %u %u %c %c %c %c\n",
      out[0],out[1],out[2],out[3],
      out[0],out[1],out[2],out[3]
      );
   #endif
}

void decodeSub64(uchar in[4], uchar out[3])
{
   out[0] = (uchar)(in[0] << 2 | in[1] >> 4);
   out[1] = (uchar)(in[1] << 4 | in[2] >> 2);
   out[2] = (uchar)(((in[2] << 6) & 0xc0) | in[3]);
   #if 0
   printf("DIN : %u %u %u %u\n",in[0],in[1],in[2],in[3]);
   printf("DOUT: %u %u %u\n",out[0],out[1],out[2]);
   #endif
}

// rc  0: ok
// rc >0: error
int encode64(uchar *psrc, int nsrc, uchar *pdst, int nmaxdst, int nlinechars)
{
   uchar in[3], out[4];
   int i=0, nlen=0, nchars = 0;
   mclear(in);
   mclear(out);

   uchar *pmaxsrc = psrc + nsrc;
   uchar *pmaxdst = pdst + nmaxdst;
 
   while (psrc < pmaxsrc)
   {
      nlen = 0;
      for (i = 0; i < 3; i++ )
      {
         if (psrc < pmaxsrc) {
            in[i] = *psrc++;
            nlen++;
         } else {
            in[i] = 0;
         }
      }
      if (nlen) {
         encodeSub64(in, out, nlen);
         for (i = 0; i < 4; i++ ) {
            if (pdst >= pmaxdst-1) {
               perr("output buffer overflow (%d)\n",nmaxdst);
               return 9;   // output buffer overflow
            }
            *pdst++ = out[i];
         }
         nchars++;
      }
      if (   (nlinechars>0 && nchars >= (nlinechars/4))
          || (psrc >= pmaxsrc)
         )
      {
         if (nchars) {
            if (pdst < pmaxdst-3) {
               *pdst++ = '\r';
               *pdst++ = '\n';
            }
         }
         nchars = 0;
      }
   }

   // zero terminate output as it's a string
   *pdst = '\0';

   return 0;
}

// wsp white space protection en/decoding

// RC is status
int wspencodedot(char *pin, int nin, char *pout, int noutmax, char pfc='.')
{
   char *poutcur=pout;
   char *poutmax=pout+noutmax;

   // sprintf(poutcur, ":wsp=%c\n", pfc);
   // poutcur += strlen(poutcur);

   char *pcur=pin;
   char *pcurmax=pin+nin;

   int istate=0;
   while (*pcur!=0 && pcur<pcurmax && poutcur+10<poutmax)
   {
      char c=*pcur;
      switch (istate)
      {
         case 0:
            *poutcur++ = pfc;
            if (c==' ') {
               istate=1;
               continue;
            }
            *poutcur++ = ' ';
            istate=2;
            continue;
 
         case 1:
            if (c==' ') {
               *poutcur++ = pfc;
               pcur++;
               continue;
            }
            *poutcur++ = ' ';
            istate=2;
            continue;

         case 2:
            *poutcur++ = *pcur++;
            if (c=='\n')
               istate=0;
            continue;
      }
   }
   *poutcur = '\0';

   return 0;
}

// RC is output length
int wspdecodedot(char *pin, int nin, char *pout, int noutmax, char pfcdef='.')
{
   char *poutcur=pout;
   char *poutmax=pout+noutmax;

   char *pcur=pin;
   char *pcurmax=pin+nin;

   char pfc=pfcdef;

   /*
   int iskip=0;
   if (!strncmp(pcur, ":wsp=", 5)) iskip = 5;
   if (!strncmp(pcur, "\n:wsp=", 5)) iskip = 6;
   if (!strncmp(pcur, "\r\n:wsp=", 5)) iskip = 7;
   if (iskip)
   {
      pcur += iskip;
      if (strlen(pcur) < 3)
         return -1;
      pfc = *pcur++;
      if (*pcur=='\r') pcur++;
      if (*pcur=='\n') pcur++;
   }
   */

   int istate=0;
   while (*pcur!=0 && pcur<pcurmax && poutcur+10<poutmax)
   {
      char c=*pcur;
      switch (istate)
      {
         case 0:
            if (c==pfc) {
               pcur++;
               istate=1;
               continue;
            }
            istate=2;
            continue;

         case 1:
            if (c==pfc) {
               *poutcur++ = ' ';
               pcur++;
               continue;
            }
            // must be a blank now
            if (c==' ')
               pcur++;
            istate=2;
            continue;

         case 2:
            *poutcur++ = *pcur++;
            if (c=='\n')
               istate=0;
            continue;
      }
   }
   *poutcur = '\0';

   return poutcur-pout;
}

// RC is status
int wspencodea0(char *pin, int nin, char *pout, int noutmax)
{
   int bprefix=0;

   char *poutcur=pout;
   char *poutmax=pout+noutmax;

   char *pcur=pin;
   char *pcurmax=pin+nin;

   int istate=0;
   while (*pcur!=0 && pcur<pcurmax && poutcur+10<poutmax)
   {
      char c = *pcur;
      switch (istate)
      {
         case 0: // lstart
            if (c=='\r' || c=='\n') {
               // empty line -> \xa0[eol]
               *poutcur++ = '\xa0';
               istate=1;
               continue;
            }
            istate=1;
            continue;
 
         case 1: // within line
            if (c==' ')
               istate=2;
            *poutcur++ = c;
            pcur++;
            if (c=='\n')
               istate=0;
            continue;

         case 2: // possible blank sequence
            if (c!=' ') {
               istate=1;
               continue;
            }
            *poutcur++ = '\xa0';
            pcur++;
            continue;
      }
   }
   *poutcur = '\0';

   return 0;
}

// RC is output length
int wspdecodea0(char *pin, int nin, char *pout, int noutmax)
{
   char *poutcur=pout;
   char *poutmax=pout+noutmax;

   char *pcur=pin;
   char *pcurmax=pin+nin;

   int istate=0;
   while (*pcur!=0 && pcur<pcurmax && poutcur+10<poutmax)
   {
      char c = *pcur;
      switch (istate)
      {
         case 0: // lstart
            if (c=='\xa0')
               pcur++;
            istate=1;
            continue;
 
         case 1: // within line
            if (c=='\xa0')
               *poutcur++ = ' ';
            else
               *poutcur++ = c;
            pcur++;
            if (c=='\n')
               istate=0;
            continue;
      }
   }
   *poutcur = '\0';

   return poutcur-pout;
}

// rc 0: no change
// else: szTopURLBuf
// uses: szLineBuf
bool encodeURL(char *pszRaw)
{
   char abToEncode[256]; // sfk1854 url blank encoding
   mclear(abToEncode);
   abToEncode[32] = 1;

   mclear(szLineBuf);

   // http://foo.com/the bar.txt
   char *ptmp = strstr(pszRaw, "//");
   if (!ptmp) return 0;

   ptmp += 2;
   ptmp = strchr(ptmp, '/');
   if (!ptmp) return 0;

   // on "/the bar.txt"
   if (!strchr(ptmp, ' ')) return 0;

   if (encodeHex((uchar*)ptmp, strlen(ptmp), szLineBuf, MAX_LINE_LEN, '%', abToEncode) < 0)
      return 0; // buffer overflow

   snprintf(szTopURLBuf, MAX_LINE_LEN, "%.*s%s",
      (int)(ptmp-pszRaw), pszRaw, szLineBuf);

   if (cs.debug)
      printf("encoded: %s\n", szTopURLBuf);

   // http://fo.com/the%20bar.txt
   return 1;
}

// return valid value+1, or 0 if it is noise.
uchar mapchar(char ch)
{
   if ((ch >= 'A') && (ch <= 'Z'))
      return (uchar)(ch - 'A')+1;
   else if ((ch >= 'a') && (ch <= 'z'))
      return (uchar)(ch - 'a' + 26)+1;
   else if ((ch >= '0') && (ch <= '9'))
      return (uchar)(ch - '0' + 52)+1;
   else if (ch == '+')
      return 62+1;
   else if (ch == '/')
      return 63+1;
   return 0;
}

// result: number of bytes decoded, or -1 on error
int decode64(uchar *psrc, int nsrc, uchar *pdst, int nmaxdst)
{
   uchar in[4], out[3], v;
   int i=0, nlen=0;
   mclear(in);
   mclear(out);
 
   uchar *pmaxsrc = psrc + nsrc;
   uchar *pmaxdst = pdst + nmaxdst;
   uchar *pdstraw = pdst;

   while (psrc < pmaxsrc) {
      for (nlen=0, i=0; i < 4 && (psrc < pmaxsrc); i++ ) {
         v = 0;
         while ((psrc < pmaxsrc) && v == 0) {
            v = *psrc++;
            v = mapchar(v);
         }
         if (psrc <= pmaxsrc) {
            if (v) {
               nlen++;
               in[i] = v - 1;
            } else {
               in[i] = 0;
            }
         } else {
            in[i] = 0;
         }
      }
      if (nlen) {
         decodeSub64(in, out);
         for (i=0; i<nlen-1; i++) {
            if (pdst >= pmaxdst)
               return -1;
            *pdst++ = out[i];
         }
      }
   }

   // no zero termination as it's a binary

   return pdst - pdstraw;
}

int execLoad(Coi *pcoi)
{__
   if (pcoi->open("rb"))
      return 9+perr("cannot load: %s",pcoi->name());

   size_t nRead = 0;
   while ((nRead = pcoi->read(abBuf,sizeof(abBuf)-10)) > 0)
   {
      if (chain.coldata && chain.colbinary && !cs.collines)
      {
         if (chain.addBinary(abBuf, nRead))
            return 9;
      }
      else if (!chain.colany())
      {
         abBuf[nRead] = '\0';

         #ifdef _WIN32
         char *psz = szPrintBufMap;
         // windows only: if output is NOT directed to file, map it to DOS charset,
         // to have filenames listed with correct umlauts etc.
         if (cs.outcconv && (cs.forcecconv || bGlblHaveInteractiveConsole))
         {
            sfkchars.stransitooem((char*)abBuf);
         }
         #endif

         fwrite(abBuf, 1, nRead, stdout);
      }
      else
      {
         uchar *pSrcCur = abBuf;
         uchar *pSrcMax = abBuf + nRead;

         int iLineLen = 0, iSubLines = 0;
 
         while (pSrcCur < pSrcMax)
         {
            for (iLineLen=0; pSrcCur+iLineLen < pSrcMax; iLineLen++)
               if (pSrcCur[iLineLen]=='\r' || pSrcCur[iLineLen]=='\n')
                  break;
 
            int iRemain = iLineLen;
            iSubLines = 0;
            if (!iLineLen)
               chain.print("\n");
            else
            while (iRemain > 0)
            {
               int iCopy = MAX_LINE_LEN - 10;
 
               if (iCopy > iRemain)
                  iCopy = iRemain;
 
               memcpy(szLineBuf, pSrcCur, iCopy);
               szLineBuf[iCopy] = '\0';
               szAttrBuf[0] = '\0';
 
               chain.addLine(szLineBuf, szAttrBuf);
 
               iSubLines++;
 
               pSrcCur += iCopy;
               iRemain -= iCopy;
            }
            if (iSubLines > 1)
               cs.nlineswrapped++;
 
            // skip EOL: CRLF or LF or CR
            if (pSrcCur+1 < pSrcMax && !strncmp((char*)pSrcCur, "\r\n", 2))
               pSrcCur += 2;
            else
            if (pSrcCur < pSrcMax && (*pSrcCur == '\n' || *pSrcCur == '\r'))
               pSrcCur++;
         }
      }
   }

   pcoi->close();

   return 0;
}

void initRandom(char *penv[])
{
   // seed randomizer with time and environment
   SFKMD5 md5;
 
   num nnow = mytime(NULL);
   md5.update((uchar*)&nnow, sizeof(nnow));

   for (int i=0; penv[i] != 0; i++)
   {
      char *psz = penv[i];
      if (strlen(psz) > 0)
         md5.update((uchar*)psz, strlen(psz));
   }
 
   uchar *pdig = md5.digest();

   uint32_t nseed = 0;
   memcpy(&nseed, pdig, sizeof(nseed));
 
   srand(nseed); // initrandom, randhex
}

#ifdef _WIN32
bool safeOpenClipboard(HWND hNewOwner, bool bquiet=0)
{
   int itry = 0;
   bool btold = 0;
   for (; itry<cs.cliptries; itry++) {
      if (OpenClipboard(hNewOwner))
         break; // ok
      if (itry+1>=cs.cliptries) {
         itry=cs.cliptries;
         break;
      }
      if (!btold && !bquiet) {
         btold=1;
         pinf("clipboard is locked, will retry. (rc=%d)\n",(int)GetLastError());
      }
      Sleep(100);
   }
   if (itry >= cs.cliptries) {
      if (!bquiet) {
         perr("failed to open clipboard. (rc=%d, %d/%d)\n",
            (int)GetLastError(), itry, cs.cliptries);
         pinf("close other apps which may be using it.\n");
      }
      return 0;
   }
   return 1;
}

int putClipboard(char *pszStr)
{__
   if (!safeOpenClipboard(0)) // toclip
      return 9+perr("clipboard #1\n");
   if (!EmptyClipboard())
      return 9+perr("clipboard #2\n");

   int nStrLen = strlen(pszStr);

   HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, nStrLen+10);
   if (hMem == NULL) return 9+perr("clipboard #3\n");

   LPTSTR pCopy = (char*)GlobalLock(hMem);
   if (pCopy)
   {
      memcpy(pCopy, pszStr, nStrLen);
      pCopy[nStrLen] = 0;
   }
   GlobalUnlock(hMem);

   HANDLE hData = SetClipboardData(CF_TEXT, hMem);

   if (hData == NULL) {
      CloseClipboard();
      return 9+perr("clipboard #4\n");
   }

   // System is now owner of hMem.
   CloseClipboard();

   return 0;
}

int putClipboardW(ushort *pWStr,int nchars)
{__
   if (!safeOpenClipboard(0)) // toclipw
      return 9+perr("clipboard #5\n");
   if (!EmptyClipboard())
      return 9+perr("clipboard #6\n");

   HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, nchars*2+8);
   if (hMem == NULL) return 9+perr("clipboard #7\n");

   LPTSTR pCopy = (char*)GlobalLock(hMem);
   if (pCopy) {
      memcpy(pCopy, pWStr, nchars*2);
      pCopy[nchars*2+0] = '\0';
      pCopy[nchars*2+1] = '\0';
   }
   GlobalUnlock(hMem);

   HANDLE hData = SetClipboardData(CF_UNICODETEXT, hMem);

   if (hData == NULL) {
      CloseClipboard();
      return 9+perr("clipboard #8\n");
   }

   // System is now owner of hMem.
   CloseClipboard();

   return 0;
}

// Windows: returns wchar data with bBinary
char *getClipboard(bool bBinary, bool bWait, bool bClear, int *pLRC, int *pNWideChars)
{
   char *pszClip = 0;
   ushort *pWClip = 0;
   int  iForm  = bBinary ? CF_UNICODETEXT : CF_TEXT;
   int  nchars = 0;

   int lRC = 0;

   while (!IsClipboardFormatAvailable(iForm)) {
      if (!bWait)
         { lRC = 1; break; }
      Sleep(250);
   }

   if (lRC == 1)
      { }
   else
   if (!safeOpenClipboard(0,cs.quiet)) // fromclip
      lRC = 2;
   else
   {
      HGLOBAL hglb = GetClipboardData(iForm);
      if (hglb == NULL) {
         perr("no clipboard data available\n");
      } else {
         char *pMem = (char*)GlobalLock(hglb);
         if (pMem != NULL) {
            if (bBinary)
               pWClip = mystrwdup((ushort*)pMem,&nchars);
            else
               pszClip = strdup(pMem);
               // nchars is not set then
         }
         GlobalUnlock(hglb);
      }

      // clear the clipboard?
      if (bClear)
         EmptyClipboard();

      CloseClipboard();

      if (!pszClip && !pWClip)
         lRC = 1;
   }

   if (pLRC) *pLRC = lRC;
   if (pNWideChars) *pNWideChars = nchars;

   if (bBinary)
      return (char*)pWClip;

   return pszClip;
}
#else
int putClipboard(char *pszStr)
{
   #ifdef MAC_OS_X
   FILE *f = popen("pbcopy", "w");
   if (!f)
      return 9+perr("pbcopy not found.");
   #else
   FILE *f = popen("xclip -selection clipboard", "w");
   if (!f)
      return 9+perr("xclip not found. (xclip -selection clipboard)");
   #endif

   int nlen = strlen(pszStr);
   int rc = fwrite(pszStr, 1, nlen, f);

   fclose(f);
 
   if (rc < nlen)
      return 5+perr("could not fully write to clipboard (%d/%d)\n",rc,nlen);

   return 0;
}

char *getClipboard(bool bBinary, bool bWait, bool bClear, int *pLRC, int *pNWideChars)
{
   #ifdef MAC_OS_X
   FILE *f = popen("pbpaste", "r");
   if (!f) {
      perr("pbpaste not found.");
      return 0;
   }
   #else
   FILE *f = popen("xclip -selection clipboard -o", "r");
   if (!f) {
      perr("xclip not found. (xclip -selection clipboard -o)");
      return 0;
   }
   #endif

   int ialloc1 = 10000;
   int iremain1= 10000;
   int iused1  = 0;
   int ialloc2 = 0;

   char *pmem1 = new char[ialloc1+100];
   if (!pmem1)
      { fclose(f); return 0; }
   pmem1[0] = '\0';
   char *pmem2 = 0;

   while (1)
   {
      int iread = fread(pmem1+iused1, 1, iremain1, f);
      // printf("read %d/%d\n",iread,iremain1);
      if (iread < 1)
         break;
      iused1 += iread;
      pmem1[iused1] = '\0';
      if (iread < iremain1)
         break;

      ialloc2 = ialloc1 * 2;
      pmem2 = new char[ialloc2+100];
      if (!pmem2) {
         fclose(f);
         delete [] pmem1;
         return 0;
      }

      memcpy(pmem2, pmem1, iused1+1);
      delete [] pmem1;
      pmem1 = pmem2;
      pmem2 = 0;
      ialloc1 = ialloc2;
      iremain1 = ialloc1 - iused1;
      // printf("%d = %d - %d\n",iremain1, ialloc2, iused1);
   }

   fclose(f);

   pmem2 = strdup(pmem1);
   delete [] pmem1;
   return pmem2;
}
#endif

bool isHelpOpt(char *psz) {
   if (!strcmp(psz, "-h"))     return 1;
   if (!strcmp(psz, "-help"))  return 1;
   if (!strcmp(psz, "-?"))     return 1;
   if (!strcmp(psz, "/?"))     return 1;
   if (!strcmp(psz, "/help"))  return 1;
   return 0;
}

#ifdef VFILEBASE
void resetLoadCaches(bool bfinal, const char *pszFromInfo)
{
   glblVCache.reset(bfinal, pszFromInfo);
   // fix: 1770: no glblConCache.reset here.
}
#endif // VFILEBASE

// lowest level output for words or full lines
int sfkprintf(const char *pszFormat, ...)
{
   // output to stdout
   va_list argList;
   va_start(argList, pszFormat);
   ::vprintf(pszFormat, argList);
   return 0;
}

bool isrs(char c, bool &rTabFlag)
{
   if (c == ' ')
      return 1;
   if (c == '\t')
      rTabFlag=1;
   return 0;
}

KeyMap glblSFKVar;

struct SFKVarHead {
   int iDataLen;
   int iAllocLen; // sfk1921 including header
};

bool sfkhavevars( )
{
   return glblSFKVar.size() ? 1 : 0;
}

bool isvarnamechar(char c, bool bforset)
{
   if (isalpha(c)) // varname
      return 1;
   if (isdigit(c))
      return 1;
   if (c=='_')
      return 1;
   if (bforset==0 && c=='.')
      return 1;
   return 0;
}

// sfk1922 addtovar support
//   nadd bit 0: append
//   nadd bit 1: without lf
//   nadd bit 2: allow internal names like run.x
int sfksetvar(char *pname, uchar *pDataIn, int iDataIn, int nadd)
{
   if (pDataIn == 0) return 9+perr("int. #2171132");
   if (iDataIn <  0) return 9+perr("int. #2171133");

   bool badd    = (nadd & 1) ? 1 : 0;
   bool bwithlf = (nadd & 2) ? 0 : 1; // sfk1934
   bool bintnam = (nadd & 4) ? 1 : 0; // sfk1973

   if (!bintnam)
   for (char *psz=pname; *psz; psz++)
   {
      if (isalpha(*psz)) // varname
         continue;
      if (isdigit(*psz)!=0 && psz>pname)
         continue;
      if (*psz=='_' && psz>pname)
         continue;

      perr("invalid variable name: %s",pname);
      pinf("must start with a-z, then a-z0-9_\n");
      return 9;
   }

   int iHeadSize = sizeof(struct SFKVarHead);

   struct SFKVarHead oOldHead;
   struct SFKVarHead oNewHead;

   uchar *pOldDataRaw = (uchar*)glblSFKVar.get(pname);
   uchar *pOldDataNet = pOldDataRaw;

   uchar *pNewDataRaw = 0;
   uchar *pNewDataNet = 0;

   int iOldAlloc=0, iOldUsed=0, iOldRemain=0;
   int iNewAlloc=0, iNewUsed=0, iAddSpace=0;

   oOldHead.iDataLen = oOldHead.iAllocLen = 0;
   oNewHead.iDataLen = oNewHead.iAllocLen = 0;
 
   if (pOldDataRaw)
   {
      // get old data
      memcpy(&oOldHead, pOldDataRaw, iHeadSize);

      pOldDataNet = pOldDataRaw + iHeadSize;
      iOldAlloc   = oOldHead.iAllocLen;
      iOldUsed    = oOldHead.iDataLen;
      iOldRemain  = iOldAlloc - iHeadSize - iOldUsed;

      // check signature
      if (pOldDataNet[iOldUsed] != 0)
         return 9+perr("sfkvar mem overwrite.1: %s\n", pname);
   }

   if (   badd == 0
       && pOldDataRaw != 0
       && iOldUsed == iDataIn
      )
   {
      // reuse var memory on exactly same length.
      memcpy(pOldDataNet, pDataIn, iDataIn);
      if (cs.debug) printf("[var.update  %05u %s]\n", iDataIn, pname);
   }
   else if (pOldDataRaw != 0 && badd == 1 && iDataIn+10 < iOldRemain)
   {
      // add to existing, by simple copy into free space

      // force lf on old if non empty
      if (bwithlf)
         if (iOldUsed > 0 && pOldDataNet[iOldUsed-1] != '\n')
            pOldDataNet[iOldUsed++] = '\n';

      // append new to old
      memcpy(pOldDataNet+iOldUsed, pDataIn, iDataIn);
      iOldUsed += iDataIn;

      // force lf on new. also force lf if new is empty.
      if (bwithlf)
         if (iOldUsed < 1 || iDataIn < 1 || pOldDataNet[iOldUsed-1] != '\n')
            pOldDataNet[iOldUsed++] = '\n';

      pOldDataNet[iOldUsed] = '\0';

      // update header
      oOldHead.iDataLen = iOldUsed;
      memcpy(pOldDataRaw, &oOldHead, iHeadSize);

      if (cs.debug) printf("[var.addfast %05u %s]\n", oOldHead.iDataLen, pname);
   }
   else
   {
      // allocate new, to copy or join
      if (badd == 0)
      {
         // do not add to old content.
         // pointer will be cleaned up below.
         iOldUsed = 0;
      }

      iAddSpace = badd ? MAX_LINE_LEN : 0;
      iNewAlloc = iHeadSize + iOldUsed + 1 + iDataIn + 1 + iAddSpace;

      if (cs.debug) printf("[var.alloc %05u %s add=%d %d %.20s]\n",
         iNewAlloc, pname, badd, iDataIn,
         dataAsTrace(pDataIn,mymin(iDataIn,20)));

      pNewDataRaw = new uchar[iNewAlloc + 4];
      if (!pNewDataRaw)
         return 9+perr("outofmem");

      uchar *pNewDataNet = pNewDataRaw + iHeadSize;

      iNewUsed = 0;

      // copy old payload, if any
      if (iOldUsed && pOldDataNet) {
         memcpy(pNewDataNet, pOldDataNet, iOldUsed);
         iNewUsed = iOldUsed;
      }
      // force lf on old if non empty
      if (badd==1 && bwithlf==1
          && (iNewUsed > 0 && pNewDataNet[iNewUsed-1] != '\n'))
         pNewDataNet[iNewUsed++] = '\n';

      // append new payload
      memcpy(pNewDataNet+iNewUsed, pDataIn, iDataIn);
      iNewUsed += iDataIn;

      // on add, force lf on new. also force lf if new is empty.
      if (badd==1 && bwithlf==1
          && (iNewUsed < 1 || iDataIn < 1 || pNewDataNet[iNewUsed-1]!='\n')
         )
         pNewDataNet[iNewUsed++] = '\n';

      pNewDataNet[iNewUsed] = '\0';

      // set header
      oNewHead.iDataLen = iNewUsed;
      oNewHead.iAllocLen = iNewAlloc;
      memcpy(pNewDataRaw, &oNewHead, iHeadSize);

      glblSFKVar.put(pname, pNewDataRaw);

      // cleanup
      if (pOldDataRaw)
         delete [] pOldDataRaw;
   }

   return 0;
}

int sfkdelvar(char *pname)
{
   // this points to internal data start with header
   uchar *pOldDataRaw = (uchar*)glblSFKVar.get(pname);
   if (pOldDataRaw == 0)
      return 5;

   // deletes internal key, but not data
   glblSFKVar.remove(pname);

   // delete data manually
   delete pOldDataRaw;

   return 0;
}

uchar *sfkgetvar(char *pname, int *plen)
{
   static char szBuf[30];

   struct SFKVarHead ohead;
   int iHeadSize = sizeof(struct SFKVarHead);

   // sfk1813: env variable access
   if (!strncmp(pname, "env.", 4) && pname[4])
   {
      char *psz = getenv(pname+4);
      if (!psz)
         return 0;
      if (plen)
         *plen = strlen(psz);
      return (uchar*)psz;
   }
   if (strBegins(pname, "sys."))
   {
      char *psz = 0;
      if (!strcmp(pname, "sys.slash")) // sfk191 #(sys.slash)
         psz = str(glblPathStr);
      else if (!strcmp(pname, "sys.sfkver")) // sfk1932 #(sys.sfkver)
         psz = str(getPureSFKVersion());
      else if (!strcmp(pname, "sys.numcols")) { // sfk1932 #(sys.numcols)
         sprintf(szBuf, "%d", nGlblConsColumns);
         psz = szBuf;
      }
      else if (!strcmp(pname, "sys.ownscript.name")) { // sfk196
         if (pGlblCurScriptName)
            psz = pGlblCurScriptName;
         else
            psz = str("");
      }
      else if (!strcmp(pname, "sys.ownscript.text")) { // sfk196
         if (pGlblCurrentScript)
            psz = pGlblCurrentScript;
         else
            psz = str("");
      }
      if (psz) {
         if (plen)
            *plen = strlen(psz);
         return (uchar*)psz;
      }
   }
   if (strBegins(pname, "opt.")) // internal
   {
      szBuf[0]='\0';
      if (!strcmp(pname, "opt.quiet"))   sprintf(szBuf, "%d", cs.quiet);
      if (!strcmp(pname, "opt.verbose")) sprintf(szBuf, "%d", cs.verbose);
      if (!strcmp(pname, "opt.upat"))    sprintf(szBuf, "%d", glblUPatMode);
      if (szBuf[0]) {
         if (plen)
            *plen = strlen(szBuf);
         return (uchar*)szBuf;
      }
   }

   uchar *pres = (uchar*)glblSFKVar.get(pname);

   if (!pres) {
      if (cs.debug)
         printf("[getvar %s finds nothing]\n",pname);
      return 0;
   }

   memcpy(&ohead, pres, iHeadSize);

   pres += iHeadSize;

   if (plen)
      *plen = ohead.iDataLen;

   if (cs.debug)
      printf("[getvar %s returns %d bytes]\n",pname,ohead.iDataLen);

   return pres;
}

// varname+1 etc.
// result data is TEMPORARY.
uchar *sfkgetvarexp(char *pname, int *plen)
{
   static char szres[100];

   char szname[100];
   char szlit[100];
   int  imaxname=sizeof(szname)-10;
   int  imaxlit=sizeof(szlit)-10;

   cchar *pszops = "+-*/";
   int ileft=0;
   for (; pname[ileft]; ileft++)
      if (strchr(pszops, pname[ileft]))
         break;
   if (!pname[ileft])
      return sfkgetvar(pname, plen);

   // from: varname+1
   // to  : 1+1
   if (ileft>=imaxname) {
      perr("variable name too large: %s\n",pname);
      return 0;
   }
   memcpy(szname, pname, ileft);
   szname[ileft] = '\0';

   char *prite=pname+ileft;

   char *pval = (char*)sfkgetvar(szname, 0);
   if (!pval) {
      perr("undefined variable '%s' within %s\n",szname,pname);
      return 0;
   }
   strcopy(szlit, pval);

   // todo: getrite

   if (strlen(szlit)+strlen(prite) > imaxlit) {
      perr("expression too large: %s\n",pname);
      return 0;
   }

   strcat(szlit,prite);

   double r=0.0;
   if (sfkcalc(r, szlit, 0, 0))
      return 0;

   if (int(r) == r) {
      sprintf(szres, "%1.0f", r);
   } else {
      sprintf(szres, "%f", r);
      char *p=szres+strlen(szres);
      while (p>szres+2 && (p[-1]=='0' && p[-2]!='.'))
         *--p = '\0';
   }
   return (uchar*)szres;
}

uchar *sfkgetvar(int i, char **ppname, int *plen)
{
   struct SFKVarHead ohead;
   int iHeadSize = sizeof(struct SFKVarHead);

   uchar *pres = (uchar*)glblSFKVar.iget(i, ppname);
   if (!pres)
      return 0;

   memcpy(&ohead, pres, iHeadSize);

   pres += iHeadSize;

   if (plen)
      *plen = ohead.iDataLen;

   return pres;
}

void sfkfreevars()
{
   for (int i=0; i<glblSFKVar.size(); i++)
   {
      uchar *pold = (uchar*)glblSFKVar.iget(i);
      if (pold)
         delete [] pold;
   }
   glblSFKVar.reset();
}

int varfuncval(char *psz)
{
   int isign=1;
   if (*psz=='-')
      { isign=-1; psz++; }
   if (isdigit(*psz))
      return isign * atoi(psz);
   char *pvar = (char*)sfkgetvar(psz,0);
   if (!pvar)
      return 0;
   return isign * atoi(pvar);
}

#ifndef USE_SFK_BASE

/*
   SFKMapArgs implements #(myvar) text insertions like in
      sfk -var setvar a=123 +echo "test: #(a) and #(strlen(a))"
*/

// variable functions.
// input must be writeable.
char *SFKMapArgs::eval(char *pszExp)
{
   char szLitBuf[200];
   szLitBuf[0] = '\0';

   // parse generics
   char *pfunc = pszExp;
   char *pnext = pfunc;
   for (; *pnext!=0 && *pnext!='('; pnext++);
   if (!*pnext) return 0;
   *pnext++ = '\0';

   char *pvarn = pnext;
   for (; *pnext!=0 && *pnext!=',' && *pnext!=')'; pnext++);
   if (!*pnext) return 0;
   char csep = *pnext; // , or )
   *pnext++ = '\0';

   int   nvart = 0;
   char *pvart = (char*)sfkgetvar(pvarn, &nvart);
   if (!pvart && strcmp(pfunc,"isset") && strcmp(pfunc,"isempty"))
   {
      pinf("undefined variable: %s\n", pvarn);
      return 0;
   }

   char *pres = 0;
   int   nres = 0;
   szClEvalOut[0] = '\0';
   bool  bdone = 0;

   do
   {
      // isset(foo)
      if (!strcmp(pfunc, "isset"))
      {
         if (csep != ')') return 0;
 
         if (pvart)
            strcpy(szClEvalOut, "1");
         else
            strcpy(szClEvalOut, "0");
 
         bdone = 1;
         break;
      }

      // isempty(foo)
      if (!strcmp(pfunc, "isempty"))
      {
         if (csep != ')') return 0;
 
         if (pvart == 0 || strlen(pvart) == 0)
            strcpy(szClEvalOut, "1");
         else
            strcpy(szClEvalOut, "0");
 
         bdone = 1;
         break;
      }

      // size(foo)
      if (!strcmp(pfunc, "size"))
      {
         if (csep != ')') return 0;
 
         snprintf(szClEvalOut, sizeof(szClEvalOut)-10,
            "%lu", (unsigned long)nvart);
 
         bdone = 1;
         break;
      }

      // strlen(foo)
      if (!strcmp(pfunc, "strlen"))
      {
         if (csep != ')') return 0;
 
         snprintf(szClEvalOut, sizeof(szClEvalOut)-10,
            "%lu", (unsigned long)strlen(pvart));
 
         bdone = 1;
         break;
      }
 
      // numlines(foo)
      if (!strcmp(pfunc, "numlines"))
      {
         if (csep != ')') return 0;

         unsigned long nlines=0;
         char *psz=pvart;
         for (; *psz; psz++)
            if (*psz=='\n') nlines++;
         if (psz>pvart && psz[-1]!='\n')
            nlines++;
 
         snprintf(szClEvalOut, sizeof(szClEvalOut)-10,
            "%lu", nlines);
 
         bdone = 1;
         break;
      }

      // strpos(foo,'bar')
      // strpos(foo,-spat '\x20')
      // strpos(foo,-case 'Bar')
      // contains(foo,'bar')
      // contains(foo,myvar) - sfk1922
      if (   !strcmp(pfunc, "strpos")
          || !strcmp(pfunc, "strrpos")
          || !strcmp(pfunc, "contains") // sfk1889
          || !strcmp(pfunc, "begins")   // sfk193
          || !strcmp(pfunc, "ends")     // sfk1973
         )
      {
         if (csep != ',') return 0;
 
         bool bcase = 0;
         bool bspat = 0;
         bool brite = strcmp(pfunc, "strrpos") ? 0 : 1;
         bool bcont = strcmp(pfunc, "contains") ? 0 : 1;
         bool bbeg  = strcmp(pfunc, "begins") ? 0 : 1;
         bool bend  = strcmp(pfunc, "ends") ? 0 : 1;
 
         while (1)
         {
            if (strBegins(pnext, "-case ")) {
               pnext += 6;
               bcase = 1;
            }
            else if (strBegins(pnext, "-spat ")) {
               pnext += 6;
               bspat = 1;
            }
            else break;
         }
 
         char *plitst = pnext;
         if (*plitst=='\'') {
            plitst++;
            pnext = plitst;
            for (; *pnext!=0 && *pnext!='\''; pnext++);
            if (!*pnext) return 0;
            *pnext++ = '\0';
            if (*pnext!=')') return 0;
 
            if (bspat) {
               if (copyFormStr(szLitBuf, sizeof(szLitBuf)-10, plitst, strlen(plitst), 2))
                  return 0;
               plitst = szLitBuf;
            }
         } else {
            // sfk1922: #(contains(foo,env.bar))
            char *pnamst = pnext;
            while (*pnext!=0 && *pnext!=')' && isvarnamechar(*pnext, 0)==1)
               pnext++;
            if (*pnext!=')' && isvarnamechar(*pnext, 0)==0)
               return 0;
            *pnext++ = '\0';
            plitst = (char*)sfkgetvar(pnamst, 0);
            if (!plitst) {
               perr("undefined variable: %s\n", pnamst);
               return 0;
            }
         }
 
         int ipos   = -1;
         char *phit = 0;

         if (bbeg) {
            // sfk193: begins with result 1 or 0
            ipos = bcase ? strbeg(pvart, plitst) : stribeg(pvart, plitst);
         } else if (bend) {
            ipos = bcase ? strEnds(pvart, plitst) : striEnds(pvart, plitst);
         } else {
            // strpos, strrpos, contains
            if (brite)
               phit = bcase ? mystrrstr(pvart, plitst) : mystrristr(pvart, plitst);
            else
               phit = bcase ? strstr(pvart, plitst) : mystrstri(pvart, plitst);
 
            if (phit)
                ipos   = (int)(phit-pvart);
 
            if (bcont)
               ipos = (ipos == -1) ? 0 : 1;
         }
 
         sprintf(szClEvalOut, "%d", ipos);

         bdone = 1;
         break;
      }
 
      // substr(foo,3[,2])
      if (!strcmp(pfunc, "substr"))
      {
         if (csep != ',') return 0;
 
         char *poff = pnext;
         for (; *pnext!=0 && *pnext!=',' && *pnext!=')'; pnext++);
         if (!*pnext) return 0;
         bool ball = (*pnext==')' ? 1 : 0);
         *pnext++ = '\0';
         int ioff = varfuncval(poff); // sfk1914
         int ntxt = strlen(pvart);
         if (ioff < 0) {
            ioff = ntxt + ioff;
            if (ioff < 0) return 0;
         }
         if (ioff > ntxt) {
            // fix sfk1920: accept offset beyond length
            ioff = ntxt;
         }
 
         int ilen = 0;
         if (ball) {
            ilen = strlen(pvart+ioff);
         } else {
            char *plen = pnext;
            for (; *pnext!=0 && *pnext!=')'; pnext++);
            if (!*pnext) return 0;
            *pnext++ = '\0';
            ilen = varfuncval(plen); // sfk1914
            if (ilen < 0) return 0;  // sfk1914
         }
 
         nres = ilen;
         pres = pvart+ioff;
         // fall through
      }
 
      if (!strcmp(pfunc, "rsubstr"))
      {
         if (csep != ',') return 0;
 
         char *poff = pnext;
         for (; *pnext!=0 && *pnext!=',' && *pnext!=')'; pnext++);
         if (!*pnext) return 0;
         bool ball = (*pnext==')' ? 1 : 0);
         *pnext++ = '\0';
         int ioff = varfuncval(poff); // sfk1914
         int ntxt = strlen(pvart);
         if (ioff < 0) {
            ioff = ntxt + ioff;
            if (ioff < 0) return 0;
         }
         if (ioff > ntxt) {
            // fix sfk1922: accept offset beyond length
            ioff = ntxt;
         }
 
         int ilen = 0;
         if (ball) {
            ilen = strlen(pvart+ioff);
         } else {
            char *plen = pnext;
            for (; *pnext!=0 && *pnext!=')'; pnext++);
            if (!*pnext) return 0;
            *pnext++ = '\0';
            ilen = varfuncval(plen); // sfk1914
         }
 
         int n    = strlen(pvart);
         if (ioff > n) ioff = n;     // safety
         char *p2 = (pvart + n) - ioff;
         if (p2 < pvart)   p2 = pvart;   // sfk1922
         if (p2 > pvart+n) p2 = pvart+n; // sfk1922

         char *p1 = p2 - ilen;
         if (ball) p1 = pvart;
         if (p1 < pvart)   p1 = pvart;   // sfk1922
         if (p1 > pvart+n) p1 = pvart+n; // sfk1922

         nres = p2-p1;
         pres = p1;
         // fall through
      }
 
      // trim(foo)
      // ltrim(foo)
      // rtrim(foo)
      if (   !strcmp(pfunc, "trim")
          || !strcmp(pfunc, "ltrim")
          || !strcmp(pfunc, "rtrim")
         )
      {
         if (csep != ')') return 0;
 
         bool bleft = 0;
         bool brite = 0;
         if (!strcmp(pfunc, "trim"))
            { bleft = brite = 1; }
         if (!strcmp(pfunc, "ltrim"))
            { bleft = 1; }
         if (!strcmp(pfunc, "rtrim"))
            { brite = 1; }
 
         char *pleft = pvart;
         char *prite = pvart+strlen(pvart);
 
         if (bleft) {
            while (*pleft==' ' || *pleft=='\t')
               pleft++;
         }
         if (brite) {
            while (prite > pleft && (prite[-1]== ' ' || prite[-1]=='\t'))
               prite--;
         }
 
         nres = prite - pleft;
         pres = pleft;
         // fall through
      }
 
      // lpad(foo,10)   "    text" - sfk1914 with variable len
      // rpad(foo,10)   "text    " - sfk1914 with variable len
      if (   !strcmp(pfunc, "lpad")
          || !strcmp(pfunc, "rpad")
         )
      {
         bool brite = strcmp(pfunc, "rpad") ? 0 : 1;
         if (csep != ',') return 0;
 
         char *ppad = pnext;
         for (; *pnext!=0 && *pnext!=')'; pnext++);
         if (!*pnext) return 0;
         *pnext++ = '\0';
         int ipad = varfuncval(ppad); // sfk1914
         int apad = abs(ipad);
         int ntxt = strlen(pvart);
         int idif = apad - ntxt;
 
         if (idif <= 0) {
            // nothing to pad
            nres = ntxt;
            pres = pvart;
         } else {
            // must fit into szClEvalOut
            if (ipad+10 > sizeof(szClEvalOut)) return 0;
 
            char *pdst = szClEvalOut;
            if (!brite) {
               for (; idif > 0; idif--)
                  *pdst++ = ' ';
            }
            while (*pvart)
               *pdst++ = *pvart++;
            if (brite) {
               for (; idif > 0; idif--)
                  *pdst++ = ' ';
            }
            *pdst = '\0';
 
            bdone = 1;
            break;
         }
      }

      if (!strcmp(pfunc, "mod")) // sfk1990 internal
      {
         if (csep != ',') return 0;

         uint nval = strtoul(pvart,0,10);

         char *pdiv = pnext;
         for (; *pnext!=0 && *pnext!=')'; pnext++);
         if (!*pnext) return 0;
         *pnext++ = '\0';
         uint ndiv = varfuncval(pdiv);

         sprintf(szClEvalOut, "%u", ndiv ? (nval % ndiv) : 0);

         bdone = 1;
         break;
      }

      if (pres)
      {
         if (nres+110 < sizeof(szClEvalOut))
         {
            if (nres) memcpy(szClEvalOut, pres, nres);
            szClEvalOut[nres] = '\0';
            bdone = 1;
            break;
         }

         // huge variable data: extra memory cache
         // and no support for +-*/
         pszClEvalOut = new char[nres+4];
         if (!pszClEvalOut) return 0;
         if (nres) memcpy(pszClEvalOut, pres, nres);
         pszClEvalOut[nres] = '\0';

         return pszClEvalOut;
      }
   }
   while (0);

   if (!bdone)
      return 0;

   #ifdef WITH_VAR_CALC
   if (*pnext!=0)
   {
      cchar *pszops = "+-*/";
      if (strchr(pszops, *pnext)!=0)
      {
         // todo: getrite
         if (strlen(szClEvalOut) + strlen(pnext) + 10 > sizeof(szClEvalOut)) {
            perr("variable expression too large: %s\n", pszExp);
            return 0;
         }
         strcat(szClEvalOut, pnext);
         double r=0.0;
         if (sfkcalc(r, szClEvalOut, 0, 0))
            return 0;
         if (int(r) == r) {
            sprintf(szClEvalOut, "%1.0f", r);
         } else {
            sprintf(szClEvalOut, "%f", r);
            char *szres=szClEvalOut;
            char *p=szres+strlen(szres);
            while (p>szres+2 && (p[-1]=='0' && p[-2]!='.'))
               *--p = '\0';
         }
      }
      else
      if (strlen(pnext) > 0 && pnext[0] != ')')
      {
         perr("invalid text '%s' in variable expression\n", pnext);
         // perr telling expression follows
         return 0;
      }
   }
   #endif

   return szClEvalOut;
}

#endif // USE_SFK_BASE

#ifdef _WIN32
char *winSysError()
{
   static CHAR szPrintBuffer[512];

   LPVOID lpvMessageBuffer;
 
   FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
          NULL, GetLastError(),
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
          (LPTSTR)&lpvMessageBuffer, 0, NULL);
 
   wsprintf(szPrintBuffer, " (%d, %s)", GetLastError(), (char *)lpvMessageBuffer);
 
   LocalFree(lpvMessageBuffer);

   return szPrintBuffer;
}
#endif

static char abLineEditPartInfo[1024];
char abLineEditPartInfo2[1024];

static ushort aswchar(char c)
{
   ushort nres = ((ushort)c) & 0xFFU;
   // if (nres != c) printf("%c -> 0x%02x\n",c,nres);
   return nres;
}

// used by rename
int pointedit(char *pszMaskIn, char *pszSrc, int *pOutMatchLen, char *pszDst, int iMaxDst, bool verb)
{
   ushort aFrom[1024];
   ushort aInfoIdx[1024];
   ushort aTo[1024];
   char   aLitPart[22][610];
   char   aPart[22][610];
   int    iPartIdx= 0, iPartIdxMax = 12-2;
   int    iPartChr= 0, iPartChrMax = 610-10;

   char *pszMask = pszMaskIn;

   int iMaskLen = strlen(pszMask);
   if (iMaskLen < 3)
      return 10+(verb?perr("/from/to/ too short"):0);

   int iFromCur = 0;
   int iFromMax = (sizeof(aFrom)/sizeof(ushort))-10;
   int iToCur   = 0;
   int iToMax   = (sizeof(aTo)/sizeof(ushort))-10;

   // --- split from/to and tokenize ---

   char csep = *pszMask++;
   char ccur = 0, csub = 0; // BEWARE of char/ushort conversion! use aswchar()
   char *pszCur = 0;

   while (*pszMask!=0 && *pszMask!=csep)
   {
      if (iFromCur >= iFromMax)
         return 11+(verb?perr("from text too long"):0);

      pszCur = pszMask;

      ccur = *pszMask++;

      if (cs.spat!=0 && ccur=='\\') {
         int iskip=1;
         ccur=0;
         switch (*pszMask) {
            case 't': ccur='\t'; break;
            case 'q': ccur='"'; break;
            case '\\': case '*': case '?':
            case '[': case ']': case '#':
               ccur=*pszMask; break;
            case 'x':
               if (pszMask[1] && pszMask[2])
                  ccur = (char)getTwoDigitHex(pszMask+1);
               iskip=3;
               break;
         }
         if (!ccur)
            return 12+(verb?perr("invalid slash pattern: %s",pszMask-1):0);
         pszMask+=iskip;
         aInfoIdx[iFromCur] = (ushort)(pszMask-1-pszMaskIn);
         aFrom[iFromCur++] = aswchar(ccur);
         continue;
      }

      if (ccur == '*') {
         aFrom[iFromCur] = 0x2000U;
         // determine stop literal and store as part
         if (iPartIdx >= 20)
            return 13+(verb?perr("too many parts"):0);
         int iLitChrIdx = pszMask-pszMaskIn;
         int iLitChrLen = 0;
         for (; pszMask[iLitChrLen]; iLitChrLen++) {
            csub = pszMask[iLitChrLen];
            if (csub==csep || csub=='*' || csub=='?')
               break;
            aLitPart[iPartIdx][iLitChrLen] = aswchar(csub);
         }
         aLitPart[iPartIdx][iLitChrLen] = '\0';
         aFrom[iFromCur] += iPartIdx;
         aInfoIdx[iFromCur] = (ushort)(pszMask-1-pszMaskIn);
         iFromCur++;
         iPartIdx++;
         continue;
      }

      if (ccur == '?') {
         int ilen = 1;
         while (*pszMask!=0 && *pszMask=='?') {
            ilen++;
            pszMask++;
         }
         aInfoIdx[iFromCur] = (ushort)(pszMask-1-pszMaskIn);
         aFrom[iFromCur++] = 0x1000U + ilen;
         continue;
      }

      if (ccur == '[') {
         // [2 chars]
         int ilen = atoi(pszMask);
         while (isdigit(*pszMask))
            pszMask++;
         if (*pszMask!=' ')
            return 14+(verb?perr("wrong [] syntax: %s",pszCur):0);
         pszMask++;
         if (strBegins(pszMask, "chars]")) {
            pszMask += strlen("chars]");
            aInfoIdx[iFromCur] = (ushort)(pszMask-1-pszMaskIn);
            aFrom[iFromCur++] = 0x1000U + ilen;
            continue;
         }
         return 15+(verb?perr("wrong [] syntax: %s",pszCur):0);
      }

      aInfoIdx[iFromCur] = (ushort)(pszMask-1-pszMaskIn);
      aFrom[iFromCur++] = aswchar(tolower(ccur));
   }
   aFrom[iFromCur] = 0;
   iFromMax = iFromCur;
   if (*pszMask!=csep)
      return 16+(verb?perr("missing end separator '%c'",csep):0);
   pszMask++;

   int iToPart = 0;

   while (*pszMask!=0 && *pszMask!=csep)
   {
      if (iToCur >= iToMax)
         return 17+(verb?perr("to text too long"):0);

      if (cs.spat!=0 && *pszMask=='\\') {
         pszMask++;
         int iskip=1;
         ccur=0;
         switch (*pszMask) {
            case 't': ccur='\t'; break;
            case 'q': ccur='"'; break;
            case '\\': case '*': case '?':
            case '[': case ']': case '#':
               ccur=*pszMask; break;
            case 'x':
               if (pszMask[1] && pszMask[2])
                  ccur = (char)getTwoDigitHex(pszMask+1);
               iskip=3;
               break;
         }
         if (!ccur)
            return 18+(verb?perr("invalid slash pattern: %s",pszMask-1):0);
         pszMask+=iskip;
         aTo[iToCur++] = aswchar(ccur);
         continue;
      }

      if (*pszMask=='#') {
         pszMask++;
         if (isdigit(*pszMask)) {
            iToPart = atoi(pszMask);
            while (isdigit(*pszMask))
               pszMask++;
         } else {
            iToPart++;
         }
         int ipart = iToPart;
         if (ipart > 0)
             ipart--;
         aTo[iToCur++] = 0x1000U + ipart;
         continue;
      }

      if (!strncmp(pszMask, "[parts ", 7))
      {
         pszMask += 7;
         bool bfill = 0;
         while (1)
         {
            // expect partno or ]
            if (isdigit(*pszMask)) {
               int ipart = atoi(pszMask);
               if (ipart < 1)
                  return 19+(verb?perr("wrong part number: %d",ipart):0);
               while (isdigit(*pszMask))
                  pszMask++;
               if (bfill) {
                  bfill=0;
                  iToPart++;
                  for (; iToPart<ipart; iToPart++)
                     aTo[iToCur++] = 0x1000U + (iToPart-1);
                  aTo[iToCur++] = 0x1000U + (ipart-1);
                  iToPart=ipart;
               } else {
                  iToPart=ipart;
                  aTo[iToCur++] = 0x1000U + (ipart-1);
               }
               continue;
            }
            if (*pszMask==',') {
               pszMask++;
               bfill=0;
               continue;
            }
            if (*pszMask=='-') {
               pszMask++;
               bfill=1;
               continue;
            }
            if (*pszMask==']') {
               pszMask++;
               break;
            }
            return 20+(verb?perr("wrong parts syntax"):0);
         }
         continue;
      }

      if (!strncmp(pszMask, "[part", 5)) {
         pszMask += 5;
         if (*pszMask==' ')
            pszMask++;
         int ipart = atoi(pszMask);
         if (ipart > 0)
             ipart--;
         iToPart=ipart+1;
         while (*pszMask!=0 && isdigit(*pszMask))
            pszMask++;
         if (*pszMask != ']')
            return 21+(verb?perr("missing ]"):0);
         pszMask++;
         aTo[iToCur++] = 0x1000U + ipart;
         continue;
      }

      aTo[iToCur++] = *pszMask++;
   }
   aTo[iToCur] = 0;
   iToMax = iToCur;
   if (*pszMask!=csep)
      return 22+(verb?perr("missing end separator '%c'",csep):0);

   pszMask++;
   if (*pszMask)
      return 23+(verb?perr("unexpected text after end separator '%c': %s",csep,pszMask):0);

   // --- match source and collect dynamic parts ---

   char *pSrcCur = pszSrc;
   int   iFrom   = 0;
   int   istate  = 0;
   int   imaxcol = 0;
   int   iLitPart= 0;

   iPartIdx = 0;
   iPartChr = 0;

   if (aFrom[iFrom] >= 0x2000U) {
      istate   = 2;
      iLitPart = aFrom[iFrom] & 0x0FFU;
      if (aLitPart[iLitPart][0])
         istate = 3;
   }
   else
   if (aFrom[iFrom] >= 0x1000U) {
      istate  = 1;
      imaxcol = aFrom[iFrom] & 0x0FFFU;
   }

   bool btrace = 0;

   if (iMaskLen+10 < sizeof(abLineEditPartInfo))
     memset(abLineEditPartInfo, ' ', iMaskLen);

   while (*pSrcCur && aFrom[iFrom])
   {
      // int ipinf = pSrcCur - pszSrc;
      // if (ipinf+10 < sizeof(abLineEditPartInfo))
      //    abLineEditPartInfo[ipinf] = '1'+(iPartIdx%10);

      ushort ninfidx = aInfoIdx[iFrom];
      if (ninfidx+10 < sizeof(abLineEditPartInfo))
         abLineEditPartInfo[ninfidx] = '1'+(iPartIdx%10);

      char csrc = *pSrcCur++;

      if (iPartIdx >= iPartIdxMax)
         return 24+(verb?perr("too many match parts"):0);
      if (iPartChr >= iPartChrMax)
         return 25+(verb?perr("match part overflow"):0);

      switch (istate)
      {
         case 0: // any or single char
            if (aFrom[iFrom] >= 0x1000U) {
               pSrcCur--;
               break;
            }
            if (aswchar(tolower(csrc)) != aFrom[iFrom]) {
               if (btrace) {
                  printf("miss %c %c\n",csrc,aFrom[iFrom]);
                  for (int i=0; i<iPartIdx; i++)
                     printf("p%d %s\n",i+1,aPart[i]);
               }
               return 1; // no match
            }
            // single char matched
            aPart[iPartIdx][iPartChr++] = csrc;
            iFrom++;
            continue;

         case 1: // group of ?
            aPart[iPartIdx][iPartChr++] = csrc;
            imaxcol--;
            if (imaxcol > 0)
               continue;
            iFrom++;
            break;

         case 2: // open *
            aPart[iPartIdx][iPartChr++] = csrc;
            continue;

         case 3: // * then literal
            // look for stop literal part
            char *psz  = pSrcCur-1;
            int   ichr = 0;
            char  c1=0, c2=0;
            for (;;ichr++) {
               c1 = tolower(psz[ichr]);
               c2 = tolower(aLitPart[iLitPart][ichr]);
               // if (btrace) printf("cmp %c %c\n",c1,c2);
               if (!c1 || !c2) break;
               if (c1 != c2) break;
            }
            if (c2) {
               aPart[iPartIdx][iPartChr++] = csrc;
               continue;
            }
            // stop literal match
            istate = 4;
            pSrcCur--;
            if (btrace) printf("stoplit match: %s\n",pSrcCur);
            iFrom++;
            break;
      }

      if (istate > 0 || iPartChr > 0) {
         aPart[iPartIdx][iPartChr] = '\0';
         iPartIdx++;
         iPartChr = 0;
      }

      if (aFrom[iFrom] >= 0x2000U) {
         istate   = 2;
         iLitPart = aFrom[iFrom] & 0x0FFU;
         if (aLitPart[iLitPart][0])
            istate = 3;
         if (btrace) printf("to state %d seeking \"%s\"\n",istate,aLitPart[iLitPart]);
      }
      else
      if (aFrom[iFrom] >= 0x1000U) {
         istate  = 1;
         imaxcol = aFrom[iFrom] & 0x0FFFU;
         if (btrace) printf("to state %d over %d chars on %s\n",istate,imaxcol,pSrcCur);
      }
      else if (aFrom[iFrom])
      {
         istate  = 0;
         if (btrace) printf("to state %d on %s mask %c\n",istate,pSrcCur,aFrom[iFrom]);
      }
   }
   if (iMaskLen+10 < sizeof(abLineEditPartInfo))
      abLineEditPartInfo[iMaskLen] = '\0';

   if (istate > 0) {
      aPart[iPartIdx][iPartChr] = '\0';
      iPartIdx++;
   }

   // is it a match?
   if (aFrom[iFrom]!=0 && istate!=2) {
      if (btrace) printf("mask not done at end of src\n");
      return 1;
   }
   // if (aFrom[iFrom]==0 && *pSrcCur!=0) {
   //    if (btrace) printf("src not done at end of mask\n");
   //    return 1;
   // }
   *pOutMatchLen = (int)(pSrcCur - pszSrc);
   if (btrace) printf("point match len=%d\n", *pOutMatchLen);

   if (btrace) {
      for (int i=0; i<iPartIdx; i++)
         printf("p%d %s\n",i+1,aPart[i]);
   }

   // --- build output from to mask and parts ---
 
   ushort *pToSrc = aTo;
   char *pDstCur = pszDst;
   char *pDstMax = pszDst+iMaxDst-10;
   while (*pToSrc)
   {
      if (pDstCur >= pDstMax)
         return 26+(verb?perr("output buffer overflow"):0);

      ushort csrc = *pToSrc++;

      if (csrc < 0x1000) {
         *pDstCur++ = (char)csrc;
         continue;
      }

      int ipart = csrc & 0xFFFU;
      if (ipart < 0 || ipart >= iPartIdxMax)
         return 27+(verb?perr("invalid part number: %d",ipart+1):0);

      char *ppart = aPart[ipart];
      while (*ppart) {
         if (pDstCur >= pDstMax)
            return 28+(verb?perr("output buffer overflow"):0);
         *pDstCur++ = *ppart++;
      }
   }
   *pDstCur = '\0';

   return 0;
}

void lineeditinit()
{
   memset(abLineEditPartInfo2, 0, sizeof(abLineEditPartInfo2));
}

char *lineeditinfo()
{
   return abLineEditPartInfo2;
}

// used by rename
int lineedit(char *pszMaskIn, char *pszSrc, char *pszDst, int iMaxDst, char *pAtt1, char *pAtt2,
   uint flags, int *poff, int *plen)
{
   bool bexact = (flags & 1) ? 1 : 0;
   bool bverb  = (flags & 2) ? 1 : 0;

   char aPointBuf[1024];
   mclear(aPointBuf);

   char *pSrcOld = pszSrc;
   char *pSrcCur = pszSrc;
   char *pDstCur = pszDst;
   char *pDstMax = pszDst+iMaxDst-10;

   bool  bAnyDone = 0;

   if (cs.debug)
      printf("match name: %s\n", dataAsTrace(pszSrc,strlen(pszSrc)));

   while (*pSrcCur)
   {
      int imatch = 0;
      int isubrc = pointedit(pszMaskIn, pSrcCur, &imatch, aPointBuf, sizeof(aPointBuf), bverb);
      if (isubrc >= 10)
         return isubrc;
      if (isubrc > 0) {
         if (bexact)
            return 1;
         if (pDstCur > pDstMax)
            return 30+perr("ledit: output overflow");
         *pDstCur++ = *pSrcCur++;
         continue;
      }
      if (imatch < 1)
         return 31+perr("ledit: invalid match");
      int iedit = strlen(aPointBuf);
      // mark match src
      if (pAtt1) memset(pAtt1+(pSrcCur-pszSrc),'i',imatch);
      // mark match dst
      if (pAtt2) memset(pAtt2+(pDstCur-pszDst),'a',iedit);
      // copy edited part
      if (pDstCur+iedit > pDstMax)
         return 32+perr("ledit: output overflow");
      memcpy(pDstCur, aPointBuf, iedit);
      pDstCur += iedit;
      if (!bAnyDone)
      {
         bAnyDone = 1;
         if (!abLineEditPartInfo2[0]) {
            memcpy(abLineEditPartInfo2, abLineEditPartInfo, sizeof(abLineEditPartInfo));
            myrtrim(abLineEditPartInfo2);
         }
         if (poff) *poff = (int)(pSrcCur-pszSrc);
         if (plen) *plen = imatch;
      }
      // skip matched source
      pSrcCur += imatch;
      if (bexact && *pSrcCur)
         return 1;
   }
   *pDstCur = '\0';

   return bAnyDone ? 0 : 1;
}

// - - - sfk modules











const char *apRunTokens[] =
{
   // new and simple     kept for compatibility     quoted expressions           new ease
   "path"    , ""      , "purepath"    , "ppath"  , "quotpath"     , "qpath"   , "",
   "file"    , ""      , "purefile"    , "pfile"  , "quotfile"     , "qfile"   , "",
   "upath"   , ""      , ""            , ""       , "quotupath"    , "qupath"  , "",
   "ufile"   , ""      , ""            , ""       , "quotufile"    , "qufile"  , "",
   "relfile" , "rfile" , "purerelfile" , "prfile" , "quotrelfile"  , "qrfile"  , "qrelfile",
   "base"    , ""      , "purebase"    , "pbase"  , "quotbase"     , "qbase"   , "",
   "ext"     , ""      , "pureext"     , "pext"   , "quotext"      , "qext"    , "",
   "since"   , ""      , "puresince"   , "psince" , "quotsince"    , "qsince"  , "",
   "text"    , ""      , ""            , ""       , "quottext"     , "qtext"   , "",
   // sfk1973
   "relpath" , ""      , ""            , ""       , "quotrelpath"  , "qrelpath", "",
   // sfk182 only with -tomake;
   "targ"    , ""      , ""            , ""       , "quottarg"     , "qtarg"   , "",
   // sfk1973 only with [f]pic
   "outext"  , ""      , ""            , ""       , ""             , ""        , "",
};

#define RUNTPR 7  // run tokens per row

// tell if a supplied user command references single files.
// if not, it will be applied on directories only.
bool anyFileInRunCmd(char *pszCmd, bool bIncludePath)
{
   char abToken[100];
   // starts at RUNPTR as path is not a single file
   uint istart = bIncludePath ? 0 : RUNTPR;
   for (uint i=istart; i<(sizeof(apRunTokens)/sizeof(apRunTokens[0])); i++)
   {
      // skip (q)targ if no -tomake
      if (!bGlblGotToMake && !strcmp(apRunTokens[i], "targ"))
         { i += (RUNTPR-1); continue; }
      strcpy(&abToken[1], apRunTokens[i]);
      if (!abToken[1]) continue;
      abToken[0] = '#';
      if (strstr(pszCmd, abToken))
         return true;
      #ifdef SFK_BOTH_RUNCHARS
      abToken[0] = '$';
      if (strstr(pszCmd, abToken))
         return true;
      #endif
   }
   return false;
}

bool anyTextInRunCmd(char *pszCmd)
{
   char abToken[100];
   for (uint i=RUNTPR*7; i<(sizeof(apRunTokens)/sizeof(apRunTokens[0])); i++)
   {
      // skip (q)targ if no -tomake
      if (!bGlblGotToMake && !strcmp(apRunTokens[i], "targ"))
         { i += (RUNTPR-1); continue; }
      strcpy(&abToken[1], apRunTokens[i]);
      if (!abToken[1]) continue;
      abToken[0] = '#';
      if (strstr(pszCmd, abToken))
         return true;
      #ifdef SFK_BOTH_RUNCHARS
      abToken[0] = '$';
      if (strstr(pszCmd, abToken))
         return true;
      #endif
   }
   return false;
}

int onRunExpression(char *psz1, int &lExpLength, bool &bquot, bool &btext)
{
   char abToken[100];
   uint nPtrs = (sizeof(apRunTokens)/sizeof(apRunTokens[0]));
   uint nRows = nPtrs / RUNTPR;
   for (uint irow=0; irow<nRows; irow++)
   {
      // skip (q)targ if no -tomake
      if (!bGlblGotToMake && !strcmp(apRunTokens[irow * RUNTPR], "targ"))
         continue;
      if (!cs.procpic && !strcmp(apRunTokens[irow * RUNTPR], "outext"))
         continue;
      for (uint icol=0; icol < RUNTPR; icol++)
      {
         strcpy(&abToken[1], apRunTokens[irow * RUNTPR +icol]);
         abToken[0] = '#';
         if (!abToken[1]) continue;
         char *psz2 = abToken;
         if (!strncmp(psz1, psz2, strlen(psz2)))
         {
            lExpLength = strlen(psz2);
            bquot = (icol >= 4) ? true : false;
            if (strstr(psz2, "text")) btext=1;
            // if (cs.debug) printf("orp %u %u - %s row %d\n", lExpLength, bquot, psz2, irow);
            return irow;
         }
         #ifdef SFK_BOTH_RUNCHARS
         else {
          abToken[0] = '$';
          if (!strncmp(psz1, psz2, strlen(psz2)))
          {
            lExpLength = strlen(psz2);
            bquot = (icol >= 4) ? true : false;
            if (strstr(psz2, "text")) btext=1;
            // if (cs.debug) printf("orp %u %u - %s row %d\n", lExpLength, bquot, psz2, irow);
            return irow;
          }
         }
         #endif
      }
   }
   // if (cs.debug) printf("orp none in %s\n", psz1);
   return -1;
}

















void tellMemLimitInfo() {
   static bool btold = 0;
   if (!btold) { btold = 1;
      pinf("some file(s) are larger than the memory limit (%d mb). see option -memlimit under \"sfk help opt\"\n",
         nGlblMemLimit / 1048576);
   }
}























// - - - sfk file walking

class ExecFileScope
{
public:
      ExecFileScope  (Coi *pCoiOrNull);
     ~ExecFileScope  ( );

static Coi *pClCoi;
};

Coi *ExecFileScope :: pClCoi = 0;

ExecFileScope::ExecFileScope(Coi *pcoi) { pClCoi=pcoi; }
ExecFileScope::~ExecFileScope( ) { pClCoi=0; }

// Function 1: check command syntax
// Function 2: create output
// RC  0 : OK all done
// RC 10 : invalid parameters
// RC 11 : unknown command
// RC 12 : missing or invalid data, cannot execute command
typedef int (*SFKMatchOutFNCallback_t)(int iFunction, char *pMask, int *pIOMaskLen, uchar **ppOut, int *pOutLen);

int cbSFKMatchOutFN(int iFunction, char *pMask, int *pIOMaskLen, uchar **ppOut, int *pOutLen)
{
   static char szOutBuf[800];

   int iCmd = 0;

   int iMaxLen = SFK_MAX_PATH+100;

   if (!strncmp(pMask, "file.name", 9)) { iCmd=1; *pIOMaskLen=9; } else
   if (!strncmp(pMask, "file.relname", 12)) { iCmd=2; *pIOMaskLen=12; }
   if (!strncmp(pMask, "file.path", 9)) { iCmd=3; *pIOMaskLen=9; }
   if (!strncmp(pMask, "file.base", 9)) { iCmd=4; *pIOMaskLen=9; }
   if (!strncmp(pMask, "file.ext" , 8)) { iCmd=5; *pIOMaskLen=8; }

   if (!iCmd)
      return 11;

   if (iFunction==1) // check syntax
   {
      if (!pOutLen)
         return 10+perr("missing outlen parameter for expression output function");
      *pOutLen = iMaxLen;
      return 0;
   }

   if (!ppOut)
      return 10+perr("invalid parameters for expression output function");

   Coi *pcoi = ExecFileScope::pClCoi;

   if (!pcoi)
      return 12+perr("missing filename data for expression output function");

   char *psz=0;

   switch (iCmd)
   {
      case 1:  // absname
         strcopy(szOutBuf, pcoi->name());
         break;
      case 2:  // relname
         strcopy(szOutBuf, pcoi->relName());
         break;
      case 3:  // path
         strcopy(szOutBuf, pcoi->name());
         if ((psz = strrchr(szOutBuf, glblPathChar)))
            *psz = '\0';
         else
            szOutBuf[0] = '\0';
         break;
      case 4:  // base
         strcopy(szOutBuf, pcoi->relName());
         if ((psz = strrchr(szOutBuf, '.')))
            *psz = '\0';
         break;
      case 5:  // ext
         if ((psz = strrchr(pcoi->relName(), '.')))
            strcopy(szOutBuf, psz+1);
         else
            szOutBuf[0] = '\0';
         break;
   }

   *ppOut = (uchar*)szOutBuf;
   *pOutLen = strlen(szOutBuf);
 
   return 0;
}

int (*pGlblCallFileDir)(Coi *pcoi) = 0;

void setFileWalkCallback(int (*pfunc)(Coi *pcoi))
{
   pGlblCallFileDir = pfunc;
}

int execCallFileDir(Coi *pcoi)
{
   if (!pGlblCallFileDir) return 9;
   return pGlblCallFileDir(pcoi);
}

// moved to sfkext:
int execInst(char *pszFileName, int lLevel, int &lFiles, int &lDirs, num &lBytes);

int execSingleFile(Coi *pcoi, int lLevel, int &lFiles, int nDirFileCnt, int &lDirs, num &lBytes, num &nLocalMaxTime, num &ntime2)
{__ _p("sf.excfile")

   mtklog(("esf fn=%d %s", nGlblFunc, pcoi->name()));

   if (cs.debug)
      { printf("execSingleFile: %s\n", pcoi->name()); }

   ExecFileScope oscope(pcoi);

   char *pszFile     = pcoi->name();
   char *pszOptRoot  = pcoi->root(1);  // raw, returns 0 if none

   if (userInterrupt())
      return 9;

   if (cs.justevery > 1) // sfk1952 internal
   {
      cs.everycnt++;
      if (cs.everycnt < cs.justevery)
         return 0;
      cs.everycnt = 0;
   }

   #ifdef SFKOFFICE
   // avoid listing the same office name many times on multiple hits
   if (cs.office && pcoi->isOfficeSubEntry()
       && useOfficeBaseNames())
   {
      Coi otmp(pcoi->name(),0);
      otmp.stripOfficeName(); // execSingleFile filter for (x)find
      if (chain.justNamesFilter
          && chain.justNamesFilter->isset(otmp.name()))
         return 0;
   }

   // -crc=x can be used only with office subentries
   if (cs.bjustcrc) {
      if (!cs.office)
         return 19+perr("-crc=x requires office reading mode");
      if (!pcoi->isOfficeSubEntry())
         return 0;
   }
   #endif // SFKOFFICE

   if (cs.preFileDelay > 0)
      doSleep(cs.preFileDelay);

   #ifdef VFILEBASE
   if (pcoi->isHttp())
   {
      if (!cs.execweb) { // exec single file
         pwarn("command does not support http. (%d,1)\n", nGlblFunc);
         return 9;
      }

      // functions that may support http read
      switch (nGlblFunc)
      {
         case eFunc_Run:
            // no preload! external process will load, but not us.
            break;

         case eFunc_JamFile:
         case eFunc_CallBack:
         case eFunc_JamIndex:
         case eFunc_SnapAdd:
         case eFunc_FileStat:
         case eFunc_FileTime:
         case eFunc_Find:
         case eFunc_GetPic:
         case eFunc_Filter:
         case eFunc_Load:
         // case eFunc_Hexdump:
         // case eFunc_ReplaceFix:
         case eFunc_ReplaceFix:  // or xfind
         // case eFunc_XHexDemo:
         // case eFunc_Version:
            if (pcoi->preload("esf", 0, 0)) // execSingleFile
               return 9+perr("failed to load: %s\n", pcoi->name());
            break;

         default:
            pwarn("command does not support http. (%d,2)\n", nGlblFunc);
            return 9;
      }
   }
   #endif // VFILEBASE

   // make sure the file really exists.
   // make an exception for "run", as it may use "$text".
   if (nGlblFunc != eFunc_Run) {
      // if no meta info was read yet, do it now.
      if (!pcoi->status())
         pcoi->readStat('e');
      // if meta reading failed, the file cannot be processed.
      if (pcoi->status() >= 9) {
         if (bGlblSFKCreateFiles) {
            uchar *pEmpty = new uchar[100];
            if (!pEmpty)
               return 9+perr("outofmem");
            memset(pEmpty, 0, 100);
            strcpy((char*)pEmpty, "[new file]\r\n");
            pcoi->setContent(pEmpty, strlen((char*)pEmpty), 123);
         } else {
            if (!cs.quiet && !cs.nowarn)
               pferr(pcoi->name(), "[warn] [nopre] cannot read: %s\n",pcoi->name());
            return 0; // do NOT stop dirtree processing
         }
      }
   }

   if (!pszOptRoot && glblFileSet.hasRoot(0))
        pszOptRoot = glblFileSet.getCurrentRoot();
   // optionally supplied root dir can still be NULL.

   info.cycle();

   // skip initial dot slash which might be returned by dir scanning
   if (!strncmp(pszFile, glblDotSlash, 2))
      pszFile += 2;

   // if set, exclude output filename from input fileset,
   // to avoid endless recursions e.g. on snapto function.
   // see also checks for FileCollectionStamp.
   if (pszGlblOutFile)
      if (equalFileName(pszGlblOutFile,pszFile))
         return 0;

   if (cs.minsize > 0 && pcoi->getSize() < cs.minsize) {
      if (nGlblTraceSel & 2) {
         setTextColor(nGlblTraceExcColor);
         info.print("file-exclude: %s too small\n", pszFile);
         setTextColor(-1);
      }
      return 0;
   }

   if (cs.maxsize > 0 && pcoi->getSize() > cs.maxsize) {
      if (nGlblTraceSel & 2) {
         setTextColor(nGlblTraceExcColor);
         info.print("file-exclude: %s too large\n", pszFile);
         setTextColor(-1);
      }
      return 0;
   }

   // -text: process only textfiles
   if (cs.textfiles || cs.binaryfiles)
   {
      bool bbin = pcoi->isBinaryFile();
      bool binc = 1;
      cchar *ptxt1 = "", *ptxt2 = "";

      if (cs.textfiles && bbin)
         { binc=0; ptxt1="binary"; ptxt2="text"; }

      if (cs.binaryfiles && !bbin)
         { binc=0; ptxt1="text"; ptxt2="binary"; }

      if (!binc) {
         if (nGlblTraceSel & 2) {
            setTextColor(nGlblTraceExcColor);
            info.print("file-exclude: %s is %s\n", pszFile, ptxt1);
            setTextColor(-1);
         }
         return 0;
      } else {
         if (nGlblTraceSel & 2) {
            setTextColor(nGlblTraceIncColor);
            info.print("file-include: %s is %s\n", pszFile, ptxt2);
            setTextColor(-1);
         }
      }
   }

   // -since: process only files newer or equal than timestamp
   if (cs.sincetime) {
      num nFileTime = pcoi->getTime();
      if (nFileTime <= 0) return 5+perr("cannot get file time: %s\n", pszFile);
      if (nFileTime > cs.maxFileTime)
         cs.maxFileTime = nFileTime;
      if (nFileTime < cs.sincetime)
         return 0; // skip
   }

   // -before: process only files older or equal than timestamp
   if (cs.untiltime) {
      num nFileTime = pcoi->getTime();
      if (nFileTime <= 0) return 5+perr("cannot get file time: %s\n", pszFile);
      if (nFileTime > cs.maxFileTime)
         cs.maxFileTime = nFileTime;
      if (nFileTime >= cs.untiltime)
         return 0; // skip
   }

   // -sincedir: process only files newer than in reference dir
   int nSinceReason = 0;
   if (pszGlblSinceDir)
   {
      // build relative name of file to be processed
      char *pszRel = pszFile;
      if (!bGlblSinceDirIncRef)
         pszRel = pcoi->rootRelName();
      if (!pszRel) return 9;
      // proceed with -sincedir processing
      joinPath(szRefNameBuf, sizeof(szRefNameBuf), pszGlblSinceDir, pszRel);
      if (cs.verbose > 1)
         printf("CMP src %s ref %s\n",pszFile,szRefNameBuf);
      mtklog(("CMP src %s ref %s",pszFile,szRefNameBuf));
      // BEWARE OF MIXUP:
      //    sfk list -sincedir foo bar
      //       means for the user: FOO (szRefNameBuf) is the SOURCE.
      // so pszFile provided in here is actually the DESTINATION for compare.
      FileStat ofsSrc, ofsDst;
      if (ofsDst.readFrom(pszFile))       // the tree processing "target"
         return 9+perr("cannot read file time: %s\n",pszFile);
      if (ofsSrc.readFrom(szRefNameBuf))  // the -sincedir "source"
      {
         // file has been added, compared to source
         nSinceReason |= 4; // added
         if (!(nGlblSinceMode & 1)) {
            // added files not included:
            if (nGlblTraceSel & 2) {
               setTextColor(nGlblTraceExcColor);
               info.print("file-exclude: %s is added file, %s does not exist\n", pszFile, szRefNameBuf);
               setTextColor(-1);
            }
            cs.addedFilesSkipped++;
            return 0; // skip added files
         }
         // added files included:
         if (nGlblTraceSel & 2) {
            setTextColor(nGlblTraceIncColor);
            info.print("file-include: %s is added file, %s does not exist\n", pszFile, szRefNameBuf);
            setTextColor(-1);
         }
      }
      else
      {
         // both files exist

         // quick-check the size
         bool bDiffSize = 0;
         if (ofsSrc.getSize() != ofsDst.getSize())
            bDiffSize = 1;

         // compare times
         int nDiffReason = 0;
         if (!bGlblIgnoreTime) {
            if (!(nDiffReason = ofsSrc.differs(ofsDst, 0))) { // NOT same if older src
               if (nGlblTraceSel & 2) {
                  setTextColor(nGlblTraceExcColor);
                  info.print("file-exclude: %s same time as %s\n", pszFile, szRefNameBuf);
                  setTextColor(-1);
               }
               // if (!bDiffSize) // fall through on size mismatch
               return 0;
            } else {
               nSinceReason |= 1;
            }
         }

         // timestamp, attrib or exist difference: check also content
         if (bDiffSize)
            nSinceReason |= 2; // different size
         else
         if (!equalFileContent(szRefNameBuf, pszFile))
            nSinceReason |= 2; // file content difference

         if (nSinceReason & 2)
         {
            // file content is different:
            if (cs.verbose)
               printf("differs by content: %s\n", pszFile);
 
            if (!(nGlblSinceMode & 2)) {
               if (nGlblTraceSel & 2) {
                  setTextColor(nGlblTraceExcColor);
                  info.print("file-exclude: %s differs against %s\n", pszFile, szRefNameBuf);
                  setTextColor(-1);
               }
               return 0; // skip differing files
            }

            // differing files included
            if (nGlblTraceSel & 2) {
               setTextColor(nGlblTraceIncColor);
               info.print("file-include: %s content differs against %s\n", pszFile, szRefNameBuf);
               setTextColor(-1);
            }
         }
         else
         {
            // have no content difference:
            if (!(nGlblSinceMode & 4)) {
               if (nGlblTraceSel & 2) {
                  setTextColor(nGlblTraceExcColor);
                  info.print("file-exclude: %s time but no content diff against %s\n", pszFile, szRefNameBuf);
                  setTextColor(-1);
               }
               return 0; // skip only-time diffs
            }

            // are we interested just in content diffs?
            if (bGlblIgnoreTime) {
               if (nGlblTraceSel & 2) {
                  setTextColor(nGlblTraceExcColor);
                  info.print("file-exclude: %s just time diff against %s\n", pszFile, szRefNameBuf);
                  setTextColor(-1);
               }
               return 0; // no content diff, no processing
            }

            // differing files included
            if (nGlblTraceSel & 2) {
               setTextColor(nGlblTraceIncColor);
               info.print("file-include: %s just time diff against %s\n", pszFile, szRefNameBuf);
               setTextColor(-1);
            }
         }
      }

      // sincedir checks passed: register reference
      pcoi->setRef(szRefNameBuf);

   }  // endif sincedir
 
   // the file is selected for processing.
   int icurfilenum = cs.selfilenum++;
   if (cs.selfilerange > 0)
   {
      // should only process part of selection:
      if (icurfilenum < cs.selfileoff)
         return 0; // before selection start, continue
      if (icurfilenum >= cs.selfileoff + cs.selfilerange) {
         // after selection end: stop silently
         cs.stopfiletree = 1;
         return 0;
      }
   }

   // -tomake specified?
   if (cs.tomake[0])
   {
      int nrc = renderOutMask(szOutNameBuf, pcoi, cs.tomake, cs.curcmd); // -to generic
      // rc 0: done with replacements
      // rc 1: done without replacements
      if (nrc >= 9) return nrc;
      if (nrc > 0)
      {
         perr("-tomake target name did not contain any patterns.\n");
         printx("<time>note : add or insert words like <run>file, <run>relfile, <run>base etc.<def>\n");
         printx("<time>note : type \"sfk run\" for a list of possible patterns.\n");
         return 9;
      }
      // check if output file exists or is older
      FileStat ofsSrc, ofsDst;
      if (ofsSrc.readFrom(pcoi->name()))
         return 9;
      if (!ofsDst.readFrom(szOutNameBuf)) {
         bool bSrcIsOlder = 0;
         bool bSameIOS = cs.syncOlder ? 0 : 1;  // same if older src?
         if (!ofsSrc.differs(ofsDst, bSameIOS, &bSrcIsOlder)) {
            if (cs.verbose)
               pinf("[nopre] skip: %s\n", pcoi->name());
            return 0;
         }
      }
      // tomake check passed: register as reference
      pcoi->setRef(szOutNameBuf);
   }

   // -to specified? if so, build output filename.
   char *pszOutFile = 0;
   if (cs.tomask)
   {
      if (cs.tomaskfile)
         pszOutFile = cs.tomask;
      else
      {
         int nrc = renderOutMask(szOutNameBuf, pcoi, cs.tomask, cs.curcmd); // -to generic
         // rc 0: done with replacements
         // rc 1: done without replacements
         if (nrc >= 9) return nrc;
         // if (nrc > 0)
         // {
         //    perr("-to target name did not contain any patterns.\n");
         //    printx("<time>note : add or insert words like <run>file, <run>relfile, <run>base etc.<def>\n");
         //    printx("<time>note : type \"sfk run\" for a list of possible patterns.\n");
         //    return 9;
         // }
         if (nrc > 0 && cs.verbose > 0) {
            printf("to single output file: %s\n", szOutNameBuf);
         }
         if ((nrc > 0) && !cs.nowarn && !strcmp(szOutNameRecent, szOutNameBuf)) {
            if (!bGlblToldAboutRecent) {
               bGlblToldAboutRecent = 1;
               pwarn("same output used twice: %s\n", szOutNameBuf);
               pinf("specify -tofile filename or -nowarn to avoid this warning.\n");
            }
         }
         strcopy(szOutNameRecent, szOutNameBuf);
         pszOutFile = szOutNameBuf;
      }
   }

   int irc = 0;

   switch (nGlblFunc)
   {
      // cmod file_md5
      #if (sfk_prog || sfk_file_md5)
      case eFunc_MD5Write  : irc = execMD5write(pcoi);   break;
      // emod
      #endif // (sfk_prog || sfk_file_md5)
      // cmod file_snap
      #if (sfk_prog || sfk_file_snap)
      case eFunc_JamFile   : irc = execJamFile(pcoi);    break;
      // emod
      #endif // (sfk_prog || sfk_file_snap)
      // cmod file_walk
      #if (sfk_prog || sfk_file_walk)
      case eFunc_CallBack  : irc = execCallFileDir(pcoi); break;
      // emod
      #endif // (sfk_prog || sfk_file_walk)
      // cmod file_tab
      #if (sfk_prog || sfk_file_tab)
      case eFunc_Detab     : irc = execDetab(pszFile, pszOutFile); break;
      case eFunc_Scantab   : irc = execScantab(pszFile);    break;
      case eFunc_Entab     : irc = execEntab(pszFile, pszOutFile); break;
      // emod
      #endif // (sfk_prog || sfk_file_tab)
      // cmod file_list
      #if (sfk_prog || sfk_file_list)
      case eFunc_FileStat  : irc = execFileStat(pcoi, lLevel, lFiles, lDirs, lBytes, nLocalMaxTime, ntime2, nSinceReason);  break;
      // emod
      #endif // (sfk_prog || sfk_file_list)
      // cmod net_ftpserv
      #if (sfk_prog || sfk_net_ftpserv)
      case eFunc_SumFiles  : irc = execSumFiles(pcoi, lLevel, lFiles, lDirs, lBytes, nLocalMaxTime, ntime2);  break;
      case eFunc_FileTime  : irc = execFileTime(pszFile);   break;
      // emod
      #endif // (sfk_prog || sfk_net_ftpserv)
      // cmod file_touch
      #if (sfk_prog || sfk_file_touch)
      case eFunc_Touch     : irc = execTouch(pszFile, 0);   break;
      // emod
      #endif // (sfk_prog || sfk_file_touch)
      // cmod file_find
      #if (sfk_prog || sfk_file_find)
      case eFunc_Find      : irc = execFind(pcoi);          break;
      // emod
      #endif // (sfk_prog || sfk_file_find)
      // cmod file_run
      #if (sfk_prog || sfk_file_run)
      case eFunc_Run       : irc = execRunFile(pcoi, pszOutFile, lLevel, lFiles, lDirs, lBytes);  break;
      // emod
      #endif // (sfk_prog || sfk_file_run)
      // cmod file_conv
      #if (sfk_prog || sfk_file_conv)
      case eFunc_FormConv  : irc = execFormConv(pszFile, pszOutFile);   break;
      // emod
      #endif // (sfk_prog || sfk_file_conv)
      // cmod file_inst
      #if (sfk_prog || sfk_file_inst)
      case eFunc_Inst      : irc = execInst(pszFile, lLevel, lFiles, lDirs, lBytes);  break;
      // emod
      #endif // (sfk_prog || sfk_file_inst)
      // cmod file_reflist
      #if (sfk_prog || sfk_file_reflist)
      case eFunc_RefColSrc : irc = execRefColSrc(pszFile);  break;
      case eFunc_RefColDst : irc = execRefColDst(pcoi);     break;
      // emod
      #endif // (sfk_prog || sfk_file_reflist)
      // cmod file_deblank
      #if (sfk_prog || sfk_file_deblank)
      case eFunc_Deblank   : irc = execDeblank(pszFile);    break;
      // emod
      #endif // (sfk_prog || sfk_file_deblank)
      // cmod net_ftp
      #if (sfk_prog || sfk_net_ftp)
      case eFunc_FTPList   : irc = execFTPList(pszFile);    break;
      case eFunc_FTPNList  : irc = execFTPNList(pszFile);   break;
      case eFunc_FTPLocList: irc = execFTPLocList(pszFile); break;
      // emod
      #endif // (sfk_prog || sfk_net_ftp)
      case eFunc_Hexdump   : irc = execHexdump(pcoi, 0, 0); break;
      // cmod file_alias
      #if (sfk_prog || sfk_file_alias)
      case eFunc_AliasList : irc = execAliasList(pszFile);  break;
      // emod
      #endif // (sfk_prog || sfk_file_alias)
      // cmod file_replace
      #if (sfk_prog || sfk_file_replace)
      case eFunc_ReplaceFix: irc = execReplace(pcoi, pszOutFile, 1); break;
      case eFunc_ReplaceVar: irc = execReplace(pcoi, pszOutFile, 0); break;
      // emod file_replace
      #endif // (sfk_prog || sfk_file_replace)
      // cmod file_filter
      #if (sfk_prog || sfk_file_filter)
      case eFunc_Filter    : irc = execFilter(pcoi, 0, 0, -1, pszOutFile); break;
      // emod file_filter
      #endif // (sfk_prog || sfk_file_filter)
      case eFunc_Load      : irc = execLoad(pcoi);          break;
      // cmod file_del
      #if (sfk_prog || sfk_file_del)
      case eFunc_Delete    : irc = execDelFile(pszFile);    break;
      // emod
      #endif // (sfk_prog || sfk_file_del)
      #ifndef USE_SFK_BASE
      // cmod file_copy
      #if (sfk_prog || sfk_file_copy)
      case eFunc_Copy      : irc = execFileCopy(pcoi);      break;
      case eFunc_Move      : irc = execFileMove(pcoi);      break;
      case eFunc_Cleanup   : irc = execFileCleanup(pszFile);      break;
      // emod file_copy
      #endif // (sfk_prog || sfk_file_copy)
      // cmod file_dupscan
      #if (sfk_prog || sfk_file_dupscan)
      case eFunc_DupScan   : irc = execDupScan(pcoi);       break;
      // emod
      #endif // (sfk_prog || sfk_file_dupscan)
      // cmod file_rename
      #if (sfk_prog || sfk_file_rename)
      case eFunc_Rename    : irc = execRename(pcoi);        break;
      case eFunc_XRename   : irc = execXRename(pcoi);       break;
      // emod
      #endif // (sfk_prog || sfk_file_rename)
      #if defined(SFKPACK)
      case eFunc_ZipTo     : irc = execZipFile(pcoi, 0, 0); break;
      #endif
      // cmod file_bintext
      #if (sfk_prog || sfk_file_bintext)
      case eFunc_UUEncode  : irc = execUUEncode(pcoi); break;
      // emod file_bintext
      #endif // (sfk_prog || sfk_file_bintext)
      #endif // USE_SFK_BASE
      // cmod file_version
      #if (sfk_prog || sfk_file_version)
      case eFunc_Version   : irc = execVersion(pcoi); break;
      // emod
      #endif // (sfk_prog || sfk_file_version)
      #ifdef SFKPIC
       // cmod prog
       #if (sfk_prog)
        case eFunc_Pic     : irc = execPic(pcoi, pszOutFile);   break;
       // emod prog
       #endif // (sfk_prog)
      #endif // SFKPIC
      // cmod audio
      #if defined(sfk_audio)
      case eFunc_Play      : irc = execPlay(pcoi, pszOutFile); break;
      // emod
      #endif // (sfk_audio)
      default:
         break;
   }

   if (cs.postFileDelay > 0)
      doSleep(cs.postFileDelay);

   return irc;
}

int execSingleDir(Coi *pcoi, int lLevel, int &nTreeFiles, FileList &oDirFiles, int &lDirs, num &lBytes, num &nLocalMaxTime, num &ntime2)
{__ _p("sf.execdir")

   char *pszName     = pcoi->name();
   char *pszOptRoot  = pcoi->root(1);  // raw, returns 0 if none

   if (cs.debug) printf("]  esdir: %s files=%d\n", pszName, nTreeFiles);

   if (userInterrupt())
      return 9;

   if (cs.withdirs || cs.justdirs)
   {
      // -justdirs: if a path mask is given, we still have
      // to traverse all subfolders, but we don't list
      // or process non matching subfolders.
      // ,1 : take full path, is a directory name
      // ,1 : apply also white masks (?)
      if (!matchesDirMask(pszName, 1, 1, 102)) // on subdir
      {
         if (cs.debug) printf("]  esdir: path mask mismatch\n");
         return 0; // filter from output
      }

      // -justdirs: IF any non-"*" file mask is set,
      if (glblFileSet.anyFileMasks())
      {
         if (cs.debug) printf("]  esdir: using file masks\n");
         //  process dir only if any files within match
         if (!nTreeFiles) return 0; // skip
         // -justdirs: make sure the next-higher dir is not listed again
         if (cs.justdirs)  // FIX: 167: stat mydir .mask always "0 files"
            nTreeFiles = 0;
      } else {
         if (cs.debug) printf("]  esdir: no file masks, process all\n");
      }
   }

   if (cs.walkDirDelay > 0) doSleep(cs.walkDirDelay);

   // skip initial dot slash which might be returned by dir scanning
   if (!strncmp(pszName, glblDotSlash, 2))
      pszName += 2;

   if (pszGlblSinceDir)
   {
      // build relative name of file to be processed
      char *pszRel = pszName;
      if (!bGlblSinceDirIncRef)
            pszRel = rootRelativeName(pszName, pszOptRoot);
      if (!pszRel) return 9;

      // check if dirs for comparison both exist
      joinPath(szRefNameBuf, sizeof(szRefNameBuf), pszGlblSinceDir, pszRel);
      if (!isDir(szRefNameBuf)) {
         // this is often a user error
         if (cs.verbose) {
            setTextColor(nGlblWarnColor);
            info.print("no reference dir: %s\n", szRefNameBuf);
            setTextColor(-1);
         }
         nGlblMissingRefDirs++;
      } else {
         nGlblMatchingRefDirs++;
      }
   }

   // -since: process only dirs newer or equal than timestamp,
   // relevant only on commands like list -withdirs -since ...
   if (cs.sincetime) {
      num nFileTime = pcoi->getTime();
      if (nFileTime <= 0) return 5+perr("cannot get dir time: %s\n", pszName);
      if (nFileTime > cs.maxFileTime)
         cs.maxFileTime = nFileTime;
      if (nFileTime < cs.sincetime)
         return 0; // skip
   }

   // -before: process only dirs older or equal than timestamp
   if (cs.untiltime) {
      num nFileTime = pcoi->getTime();
      if (nFileTime <= 0) return 5+perr("cannot get dir time: %s\n", pszName);
      if (nFileTime > cs.maxFileTime)
         cs.maxFileTime = nFileTime;
      if (nFileTime >= cs.untiltime)
         return 0; // skip
   }

   int lRC = 0;
   switch (nGlblFunc)
   {
      // cmod file_list
      #if (sfk_prog || sfk_file_list)
      case eFunc_FileStat:
           lRC = execDirStat(pcoi, lLevel, nTreeFiles, lDirs, lBytes, nLocalMaxTime, ntime2);
           break;
      // emod
      #endif // (sfk_prog || sfk_file_list)
      // cmod net_ftpserv
      #if (sfk_prog || sfk_net_ftpserv)
      case eFunc_FileTime:
           lRC = execDirTime(pszName);
           break;
      // emod
      #endif // (sfk_prog || sfk_net_ftpserv)
      // cmod file_walk
      #if (sfk_prog || sfk_file_walk)
      case eFunc_CallBack  : lRC = execCallFileDir(pcoi); break;
      // emod
      #endif // (sfk_prog || sfk_file_walk)
      // cmod file_run
      #if (sfk_prog || sfk_file_run)
      case eFunc_Run     :
           if (cs.justdirs)
              lRC = execRunDir(pcoi, lLevel, nTreeFiles, lDirs, lBytes);
           break;
      // emod
      #endif // (sfk_prog || sfk_file_run)
      // cmod file_deblank
      #if (sfk_prog || sfk_file_deblank)
      case eFunc_Deblank :
           return execDeblank(pszName);
           break;
      // emod
      #endif // (sfk_prog || sfk_file_deblank)
      #ifndef USE_SFK_BASE
      // cmod file_copy
      #if (sfk_prog || sfk_file_copy)
      case eFunc_Copy    :
           return execDirCopy(pszName, oDirFiles);
      case eFunc_Cleanup :
           return execDirCleanup(pszName, oDirFiles);
      case eFunc_Move    :
           return execDirMove(pszName, oDirFiles);
      // emod file_copy
      #endif // (sfk_prog || sfk_file_copy)
      #if defined(SFKPACK)
      case eFunc_ZipTo:  return execZipFile(pcoi, 1, 0); break;
      #endif
      #endif // USE_SFK_BASE

      case eFunc_Find:
           if (cs.subdirs) {
              info.setAddInfo("%u files, %u dirs", cs.filesScanned, cs.dirsScanned);
              info.setStatus("scan ", pszName, 0, eKeepAdd);
              cs.dirsScanned++;
           }
           break;

      // cmod file_del
      #if (sfk_prog || sfk_file_del)
      case eFunc_Delete:
            lRC = execDelDir(pszName, lLevel, nTreeFiles, oDirFiles, lDirs, lBytes, nLocalMaxTime, ntime2);
            break;
      // emod
      #endif // (sfk_prog || sfk_file_del)

      // cmod net_ftp
      #if (sfk_prog || sfk_net_ftp)
      case eFunc_FTPList :
            if (!bGlblFTPListAsHTML || cs.withdirs)
               return execFTPList(pszName);
            break;
      // emod
      #endif // (sfk_prog || sfk_net_ftp)

      // cmod file_rename
      #if (sfk_prog || sfk_file_rename)
      case eFunc_XRename:
         if (cs.withdirs || cs.justdirs)
            return execXRename(pcoi);
         break;
      // emod file_rename
      #endif // (sfk_prog || sfk_file_rename)

      default:
           break;
   }

   // localMaxTime was used w/in above methods, reset now
   // to avoid potential further use in other dir trees.
   nLocalMaxTime = 0;

   return lRC;
}

// - - - sfk program



// - - - general cleanup

void shutdownAllGlobalData()
{__
   mtklog(("shutdownAllGlobalData begin"));

   // final cleanup of tmp cmd data
   cleanupTmpCmdData();

   if (pGlblDumpBuf) delete [] pGlblDumpBuf;

   #ifdef WITH_BITFILTER
   glblBitFilter.shutdown();
   #endif // WITH_BITFILTER

   // cleanup for all commands
   glblFileSet.shutdown();
   glblCircleMap.reset();


   #ifdef VFILEBASE
   resetLoadCaches(1, "sd");
   glblConCache.reset(1, "sd");
   #endif // VFILEBASE

   if (pGlblFileParms)  delete pGlblFileParms;
   if (apGlblFileParms) delete [] apGlblFileParms;
   if (pszGlblJamRoot)  delete [] pszGlblJamRoot;
   if (pszGlblDstRoot)  delete [] pszGlblDstRoot;
   if (pszGlblSinceDir) delete [] pszGlblSinceDir;
   if (pszGlblDirTimes) delete [] pszGlblDirTimes;

   chain.shutdown();
   // cmod file_filter
   #if (sfk_prog || sfk_file_filter)
   gfiltPreContext.shutdown();
   // emod file_filter
   #endif // (sfk_prog || sfk_file_filter)
   // dmod sfk_prog
   #if (sfk_prog)
   glblSynTests.resetEntries();
   // emod sfk_prog
   #endif // (sfk_prog)

   #ifndef USE_SFK_BASE // sfk180
    // cmod text_match
    #if (sfk_text_match)
     SFKMatch::shutdown();
    // emod text_match
    #endif // (sfk_text_match)
   #endif // USE_SFK_BASE

   #ifndef SWINST
   sfkfreevars();
   #endif

   #ifdef SFK_CCDIRTIME
   if (glblCreatedDirs.numberOfEntries() > 0)
      if (cs.verbose >= 1) {
         pwarn("createddirs list has remaining entries:\n");
         glblCreatedDirs.dump();
      }
   glblCreatedDirs.resetEntries();
   #endif

   #ifdef _WIN32
   // used by getFileMD5NoCache, was alloc'ed on demand:
   if (pGlblMD5NoCacheBuf != 0)
      VirtualFree(pGlblMD5NoCacheBuf, nGlblMD5NoCacheBufSize, MEM_DECOMMIT);
   #endif

   #ifdef VFILEBASE
   // in case any tcp was used
   TCPCore::sysCleanup();
   #endif // VFILEBASE

   #if (defined(WITH_TCP) || defined(VFILENET) || defined(DV_TCP))
   shutdownTCP();
   #endif

   mtklog(("shutdownAllGlobalData done"));
}

void cleanupTmpCmdData()
{__
   mtklog(("cleanupTmpCmdData"));
   glblGrepPat.reset();
   glblUnzipMask.reset();
   glblIncBin.reset();
   glblSFL.resetEntries();
   glblFileListCache.resetEntries();

   #ifndef USE_SFK_BASE
   // dmod net_ftp
   #if (sfk_prog || sfk_net_ftp)
   glblFTPRemList.resetEntries();
   glblFTPLocList.resetEntries();
   // emod
   #endif // (sfk_prog || sfk_net_ftp)
   // cmod file_copy
   #if (sfk_prog || sfk_file_copy)
   filedb.reset();
   // emod file_copy
   glblVerifier.reset();
   #endif // (sfk_prog || sfk_file_copy)
   #endif

   glblErrorLog.resetEntries();
   glblStaleLog.resetEntries();
}

// - - - extra modules


#ifdef VFILEBASE
// internal check: structure alignments must be same as in sfkext.cpp
void getAlignSizes1(int &n1, int &n2, int &n3)
{
   n1 = sizeof(AlignTest1);
   n2 = sizeof(AlignTest2);
   n3 = sizeof(AlignTest3);
}
#endif // VFILEBASE
