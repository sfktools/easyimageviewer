
#define USE_SFT    // sft support in ftp server
#define USE_DCACHE // mainly for dview net files
#define REDUCE_CACHE_FILE_NAMES

#if !defined(VER_STR_OS) && defined(_WIN32)
 #define _WIN32_WINNT 0x0400 // for copyFileEx
#endif

#define FOR_SFK_INCLUDE
#include "sfkbase.hpp"

#if (defined(SFKWEB) || defined(SFKPIC) || defined(SFK_PROFILING))
 #include "sfkint.hpp"
#endif



// in case of linking problems concerning libsocket etc.,
// you may out-comment this to compile without tcp support:
#ifndef USE_SFK_BASE
 #define WITH_TCP
 #define SFK_FTP_TIMEOUT "30" // seconds, as string
#endif // USE_SFK_BASE

#define snprintf  mysnprintf
#define sprintf   mysprintf

// just close on a socket is not enough.
// myclosesocket also does the shutdown().
#define closesocket myclosesocket

#ifdef _WIN32
 #ifdef SFK_MEMTRACE
  #define  MEMDEB_JUST_DECLARE
  #include "memdeb.cpp"
 #endif
#endif

#include "sfkext.hpp"

#ifndef SO_REUSEPORT
 #define SO_REUSEPORT 15
#endif

int iGlblWebCnt=0;
FILE *fGlblWebDump=0;



#ifdef WITH_SSL

// ssl.step.0 link to latest libopenssl V1.1
int bGlblSSLInitDone = 0;

void prepareSSL()
{
   if (bGlblSSLInitDone)
      return;
   bGlblSSLInitDone = 1;

   // ssl.step.1 algorithms
   SSL_load_error_strings();
   SSLeay_add_ssl_algorithms();
}

#endif // WITH_SSL

#ifndef USE_SFK_BASE

#endif // USE_SFK_BASE

extern unsigned char abBuf[MAX_ABBUF_SIZE+100];
extern struct CommandStats cspre;
extern Array glblGrepPat;
extern Array glblUnzipMask;

#ifdef _WIN32
bool vname();
#endif

extern cchar *pszGlblBlank;
int printx(const char *pszFormat, ...);
int esys(const char *pszContext, const char *pszFormat, ...);
extern cchar *arcExtList[];
void tellMemLimitInfo();
int quietMode();
char *getHTTPUserAgent();
int getTwoDigitHex(char *psz);
extern char *ipAsString(struct sockaddr_in *pAddr, char *pszBuffer, int iBufferSize, uint uiFlags=0);
extern char *ipAsString(uint ip, int iport=0);
void printCopyCompleted(char *pszName, uint nflags);
void setTextColor(int n, bool bStdErr=0, bool bVerbose=0);
bool endsWithColon(char *pszPath);
int cloneAttributes(char *pszSrc, char *pszDst, int nTraceLine);
bool canWriteFile(char *pszName, bool bTryCreate);
int setWriteEnabled(char *pszFile);
void oprintf(cchar *pszFormat, ...);
void oprintf(StringPipe *pOutData, cchar *pszFormat, ...);
bool iseol(char c);
int loadInput(uchar **ppInText, char **ppInAttr, num *pInSize, bool bstdin, char *pszInFile, bool bColor);
int loadInputFilenames(uchar **ppInText, num *pInSize, bool bstdin=0, char *pszInFile=0);
FILE *openOutFile(int &rrc);
int pferr(const char *pszFile, const char *pszFormat, ...);
void encodeSub64(uchar in[3], uchar out[4], int nlen);
void decodeSub64(uchar in[4], uchar out[3]);
uchar mapchar(char ch);
bool validFromIPMask(char *pszmask);
bool encodeURL(char *pszRaw);
int execHttpLog(uint nPort, char *pszForward, int nForward);
int cbSFKMatchOutFN(int iFunction, char *pMask, int *pIOMaskLen, uchar **ppOut, int *pOutLen);
void copySFKMatchOptions();
int createOutDirTreeW(char *pszOutFile, KeyMap *pOptMap, bool bForDir=0);
int renderOutMask(char *pDstBuf, Coi *pcoi, char *pszMask, cchar *pszCmd, bool bUniPath=0);
int dumpOutput(uchar *pOutText, char *pOutAttr, num nOutSize, bool bHexDump);
bool anyHiCodes(char *psz);
int makeServerSocket(
   uint  &nNewPort,                 // i/o parm
   struct sockaddr_in &ServerAdr,   // i/o parm
   SOCKET &hServSock,
   cchar  *pszInfo,
   uint  nAltPort=0                 // e.g. 2121 for ftp
   );
num getCurrentTicks();
void initRandom(char *penv[]);
int callLabel(char *pScript, int argc, char *argx[], char *penv[],
   char *pszLabel, int iLocalParm, int nLocalParm,
   int &lRC, bool &bFatal);
bool flexMatch(char *pszhay, cchar *pszpat);
char *getAbsPathStart(char *pszin);
int nextLine(char **pp);
uint currentKBPerSec();
uint lastErrno();
extern bool glblUPatMode;

#ifdef _WIN32
   #define mypopen _popen
   #define mypclose _pclose
#else
   #define mypopen popen
   #define mypclose pclose
#endif

static const char aenc64loc[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
   "0123456789+/";



#ifdef SFKWINST
int makeDeskIcon(HWND hwnd, char *pszTarget, char *pszShortCutName,
   char *pszWorkDir, char *pszArgs, bool bDelete);
int installSFK(char *pszFolder, bool byes);
#endif // SFKWINST

#ifdef SFKPIC
#define MAX_RECOL_COLORS 128
uint nGlblRecol = 0;
uint aGlblRecolSrc[MAX_RECOL_COLORS+4];
uint aGlblRecolDst[MAX_RECOL_COLORS+4];
#endif // SFKPIC

extern CoiTable glblFileListCache;

extern num  nGlblMemLimit;
extern int nGlblActiveFileAgeLimit;
extern bool bGlblIgnoreTime     ;
extern bool bGlblIgnore3600     ;
extern bool bGlblHexDumpWide    ;
extern int nGlblHexDumpForm     ;
extern num  nGlblHexDumpOff     ;
extern num  nGlblHexDumpLen     ;
extern char  *pszGlblCopySrc    ;
extern char  *pszGlblCopyDst    ;
extern uchar *pGlblWorkBuf      ;
extern num    nGlblWorkBufSize  ;
extern int   nGlblCopyStyle     ;
extern int   nGlblCopyShadows   ;
extern int   nGlblConsRows      ;
extern int   nGlblConsColumns   ;
extern num   nGlblShadowSizeLimit;
extern bool  bGlblUseCopyCache  ;
extern bool  bGlblShowSyncDiff  ;
extern num   nGlblMemLimit      ;
extern bool  bGlblMemLimitWasSet ;
extern bool  bGlblNoMemCheck     ;
extern char  *pGlblCurrentScript ;

extern int nGlblHeadColor      ;
extern int nGlblExampColor     ;
extern int nGlblFileColor      ;
extern int nGlblLinkColor      ;
extern int nGlblHitColor       ;
extern int nGlblRepColor       ;
extern int nGlblErrColor       ;
extern int nGlblWarnColor      ;
extern int nGlblPreColor       ;
extern int nGlblTimeColor      ;
extern int nGlblTraceIncColor  ;
extern int nGlblTraceExcColor  ;

extern bool bGlblRandSeeded;


// sfk1972 FROM HERE ON, ALL fread() and fwrite() calls are MAPPED to SAFE versions
// to work around Windows runtime bugs (60 MB I/O bug, stdin joined lines etc.)

size_t safefread(void *pBuf, size_t nBlockSize, size_t nBufSize, FILE *fin);
size_t safefwrite(const void *pBuf, size_t nBlockSize, size_t nBufSize, FILE *fin);

#define fread  safefread
#define fwrite safefwrite

// for num
unum getFlexNum(char *psz, bool bAllHex)
{
   /*
      123
      123.123.123.123
      010101011101010
      010.011.101.010
      ab2f37e9
      0x123
      0o234
      0b100
   */

   // pass 1: get overall type
   bool bbin=0,bdig=0,bxdig=bAllHex,bdots=0,bchar=0;
   if (strBegins(psz, "0x"))
      { bxdig=1; psz+=2; }
   else
   if (strBegins(psz, "0b"))
      { bbin=1; psz+=2; }
   else
   if (strBegins(psz, "0t"))
      { bchar=1; psz+=2; }
   else
   for (char *psz2=psz; *psz2; psz2++) {
      char c = tolower(*psz2);
      if (isdigit(c))
         { bdig=1; continue; }
      if (isxdigit(c))
         { bxdig=1; continue; }
      if (c=='0' || c=='1')
         { bbin=1; continue; }
      if (c=='.')
         { bdots=1; continue; }
      bchar=1;
   }

   int ibase=1;
   if (bxdig) ibase=16;
   else
   if (bdig)  ibase=10;
   else
   if (bbin)  ibase=2;

   // pass 2: collect number(s)
   unum ncur=0,nout=0;
   for (; *psz; psz++) {
      char c = tolower(*psz);
      if (c=='.') {
         nout = nout + ncur;
         nout = nout * 256;
         ncur = 0;
         continue;
      }
      if (bchar) {
         ncur = ncur * 256;
         ncur = ncur + (unum)(*psz);
         continue;
      }
      ncur = ncur * ibase;
      switch (c) {
         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            ncur = ncur + ((unum)c - '0');
            continue;
         case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            ncur = ncur + 10 + ((unum)c - 'a');
            continue;
      }
   }

   nout = nout + ncur;

   return nout;
}

void detabLine(char *pszIn, char *pszOut, uint lMaxOut, int nTabSize)
{
   uint nInsert=0, iout=0;
   for (int icol=0; (pszIn[icol]!=0) && (iout<lMaxOut-1); icol++)
   {
      char c1 = pszIn[icol];
      if (c1 == '\t') {
         nInsert = nTabSize - (iout % nTabSize);
         for (uint i2=0; i2<nInsert; i2++)
            pszOut[iout++] = ' ';
      } else {
         pszOut[iout++] = c1;
      }
   }
   pszOut[iout] = '\0';
}









#ifdef VFILEBASE
CoiTable &CoiData::elements( )
{
   if (!pelements) pelements = new CoiTable();
   return *pelements;
}

StringMap &CoiData::headers( )
{
   if (!pClHeaders) pClHeaders = new StringMap();
   return *pClHeaders;
}

StringMap &Coi::headers( ) {
   return data().headers();
}

char *Coi::header(cchar *pname) {
   if (!hasData()) return 0;
   if (!data().pClHeaders) return 0;
   return data().headers().get((char*)pname);
}

// TODO: exakte definition was sameDomain eigentlich macht!
// ACHTUNG was will der aufrufer bei skiplen!
bool sameDomainNew(char *psz1inpre, char *psz2inpre, int &rskiplen1)
{
   char szin1[300];
   char szin2[300];

   // enforce: foo.com -> foo.com/
   strcopy(szin1, psz1inpre); szin1[280]='\0';
   strcopy(szin2, psz2inpre); szin2[280]='\0';
   strcat(szin1, "/"); // sfk1920 getpic -dir ...
   strcat(szin2, "/");
   char *psz1in = szin1;
   char *psz2in = szin2;

   char *psz1  = psz1in;
   char *psz2  = psz2in;
   char *edom1 = 0;
   char *edom2 = 0;
   char *psla1 = 0;
   char *psla2 = 0;

   if (strBegins(psz1, "http://"))  psz1 += 7;
   else
   if (strBegins(psz1, "https://")) psz1 += 8;
   else {
      if (cs.debug) printf("samedom.fail.1\n");
      return 0;
   }

   if (strBegins(psz2, "http://"))  psz2 += 7;
   else
   if (strBegins(psz2, "https://")) psz2 += 8;
   else {
      if (cs.debug) printf("samedom.fail.2\n");
      return 0;
   }

   // ignore any www.
   if (striBegins(psz1, "www.")) psz1 += 4;
   if (striBegins(psz2, "www.")) psz2 += 4;

   // isolate main domains from foo.com/ or foo.com:80/
   // todo: foo.com vs foo.com/bar.txt
   for (; *psz1!=0; psz1++) {
      if (*psz1==':' || *psz1=='/') {
         if (!edom1) edom1 = psz1-1;
      }
      if (*psz1=='/') {
         if (!psla1) psla1 = psz1;
      }
   }
   for (; *psz2!=0; psz2++) {
      if (*psz2==':' || *psz2=='/') {
         if (!edom2) edom2 = psz2-1;
      }
      if (*psz2=='/') {
         if (!psla2) psla2 = psz2;
      }
   }

   if (!edom1 || !edom2 || !psla1 || !psla2) {
      if (cs.debug) printf("samedom.fail.3 %p %p %p %p\n",edom1,edom2,psla1,psla2);
      if (cs.debug) printf("dom1: %s\n", psz1in);
      if (cs.debug) printf("dom2: %s\n", psz2in);
      return 0;
   }

   // set this to last char of domain name
   psz1 = edom1;
   psz2 = edom2;

   // step 2 dots back, or until /
   int ndots = 2;
   for (; psz1 > psz1in; psz1--) {
      if (*psz1 == '.' && !(--ndots)) break;
      if (*psz1 == '/') break;
   }
   char *pdom1 = psz1+1;
   int  nlen1 = edom1 - pdom1;

   // same on psz2
   ndots = 2;
   for (; psz2 > psz2in; psz2--) {
      if (*psz2 == '.' && !(--ndots)) break;
      if (*psz2 == '/') break;
   }
   char *pdom2 = psz2+1;
   int  nlen2 = edom2 - pdom2;

   if (nlen1 != nlen2) {
      if (cs.debug) printf("samedom.fail.4\n");
      return 0;
   }

   bool brc = strncmp(pdom1, pdom2, nlen1) ? 0 : 1;

   if (brc) rskiplen1 = (int)((psla1 + 1) - psz1in);

   if (cs.debug) printf("samedom.fail.5\n");

   return brc;
}

// list of non-traversable binary extensions
cchar *apBinExtList[] = {
   "jpg","gif","png",
   0 // EOD
};

int Coi::setTypeFromHeaders( )
{
   StringMap *pheads = &headers();
   if (!pheads)
      return 5;

   char *pctype = pheads->get(str("content-type"));

   if (!pctype)
      return 5;

   return setTypeFromContentType(pctype);
}

int Coi::setTypeFromContentType(char *pctype)
{
   bClWebText=0;
   bClWebBinary=0;
   bClWebPage=0;
   bClWebJpeg=0;
   bClWebPNG=0;
   bClWebImage=0;
 
   if (striBegins(pctype, "image/jpeg")) {
      bClWebImage=1; bClWebJpeg=1;
      bClWebBinary=1; bClWebText=0;
      setBinaryFile(1);
      return 0;
   }
   if (striBegins(pctype, "image/png")) {
      bClWebImage=1; bClWebPNG=1;
      bClWebBinary=1; bClWebText=0;
      setBinaryFile(1);
      return 0;
   }
   if (   striBegins(pctype, "image/")
       || striBegins(pctype, "audio/")
       || striBegins(pctype, "video/")
       || striBegins(pctype, "application/x-msdos-program")
       || striBegins(pctype, "application/pdf")
       || striBegins(pctype, "application/zip")
      )
   {
      bClWebBinary=1; bClWebText=0;
      setBinaryFile(1);
      return 0;
   }
   if (   striBegins(pctype, "text/html")
       || striBegins(pctype, "application/xhtml")
       || striBegins(pctype, "application/rss+xml")
      )
   {
      bClWebText=1; bClWebPage=1;
      setBinaryFile(0);
      return 0;
   }
   if (   striBegins(pctype, "text/")
       || striBegins(pctype, "application/javascript")
       || striBegins(pctype, "application/x-javascript")
       || striBegins(pctype, "application/json")
       || striBegins(pctype, "application/xml")
      )
   {
      bClWebText=1;
      setBinaryFile(0);
      return 0;
   }
   if (  (strBegins(pctype, "application/") && strstr(pctype, "zip"))
       || strBegins(pctype, "application/x-tar")
       || strBegins(pctype, "application/x-compressed")
      )
   {
      setBinaryFile(1); // is binary
      setArc(1);        // but also an archive
      return 0;
   }

   return 5;
}

int Coi::preloadFromWeb( )
{
   num nMaxSize = cs.maxwebsize;

   if (nMaxSize < 1000000)
       nMaxSize = 1000000;

   uchar *pLoadBuf  = new uchar[nMaxSize+1000];
   if (!pLoadBuf)
      return 9+perr("out of memory");

   int irc = 0;

   int iopenrc=0,iopenrc2=0;
   if ((iopenrc = open("rb")))
   {
      #ifndef SFKWEB
      return 5;
      #endif
   }

   char szCType[100]; mclear(szCType);

   int  iTotalRead  = 0;

   if ((nClHave & COI_HAVE_BINARY) == 0)
   {
      mtklog(("read probe"));

      // we have ZERO information. read probe.
      memset(pLoadBuf, 0, 1024);

      iTotalRead = read(pLoadBuf, 1024);

      if (iTotalRead > 0) {
         if (fGlblWebDump) {
            fprintf(fGlblWebDump, "-----probe.beg-----\n");
            fwrite(pLoadBuf,1,mymin(128,iTotalRead),fGlblWebDump);
            fprintf(fGlblWebDump, "-----probe.done-----\n");
            fflush(fGlblWebDump);
         }
         pLoadBuf[iTotalRead]='\0';
         // detect binary by content
         uchar *p = pLoadBuf;
         if (memchr(pLoadBuf, 0, iTotalRead)) {
            // detect image by content
            if (p[1]=='P' && p[2]=='N' && p[3]=='G') {
               bClWebImage=1; bClWebPNG=1;
               bClWebBinary=1; bClWebText=0;
               setBinaryFile(1);
            } else if (p[0]==0xFF && p[1]==0xD8) {
               bClWebImage=1; bClWebJpeg=1;
               bClWebBinary=1; bClWebText=0;
               setBinaryFile(1);
            } else {
               bClBinary = 1;
            }
         } else {
            bClBinary = 0;
         }
      } else {
         bClBinary = 0;
      }
      nClHave |= COI_HAVE_BINARY;

      bClWebBinary = bClBinary;
      bClWebText   = bClWebBinary ? 0 : 1;
   }

   // complete loading of object contents
   int iMaxRead = nMaxSize;

   do
   {
      if (nClHave & COI_HAVE_SIZE)
      {
         if (getSize() > iMaxRead)
         {
            pwarn("file too large: %s (%dm)\n", name(), (int)getSize()/1000000);
            pinf("use option -weblimit=n to change limit.\n");
            irc = 5;
            break;
         }
         iMaxRead = getSize();
      }
 
      while (iTotalRead < iMaxRead)
      {
         // it may be important to read all in one call
         int iRemainSpace = iMaxRead - iTotalRead;
         int iBytes = read(pLoadBuf+iTotalRead, iRemainSpace);
         if (iBytes <= 0)
            break;
         iTotalRead += iBytes;
      }
 
      // store a copy in coi which becomes owner
      uchar *ptmp = new uchar[iTotalRead+10];

      if (!ptmp)
         return 9+perr("outofmem");

      memcpy(ptmp, pLoadBuf, iTotalRead);
      ptmp[iTotalRead] = '\0';

      setContent(ptmp, iTotalRead, nClMTime);

      mtklog(("preload sets CACHED DATA with %d bytes", iTotalRead));
   }
   while (0);

   close();
 
   delete [] pLoadBuf;

   return irc;
}

// vfile processing caches
CoiMap glblVCache;      // downloaded zip's cache
ConCache glblConCache;  // ftp and http connection cache

#define COI_CAPABILITY_NET (1UL<<0)
uint nGlblCoiConfig = 0xFFFFUL;

class DiskCacheConfig {
public:
   DiskCacheConfig ( );

   char *getPath   ( );
   int  setPath   (char *ppath);

   bool  getActive ( );
   void  setActive (bool byesno);

   char szPath[SFK_MAX_PATH+10];
   bool bactive;
};

DiskCacheConfig::DiskCacheConfig()
{
   memset(this, 0, sizeof(*this));
   // is active by default,
   // but TEMP will be accessed on demand.
   bactive = 1;
}

// guarantees non-void pointer
char *DiskCacheConfig::getPath( )
{
   if (szPath[0]) return szPath;

   char *ptmp = getenv("TEMP");

   if (!ptmp)
         ptmp = getenv("TMP");

   if (!ptmp) {
      bactive = 0;
      static bool btold1 = 0;
      if (!btold1) {
         btold1 = 1;
         if (infoAllowed())
            pinf("cannot cache to disk: no TEMP variable found.\n");
      }
      return szPath; // i.e. return empty
   }

   // set: "c:\temp\00-sfk-cache\"
   snprintf(szPath, sizeof(szPath)-10, "%s%c00-sfk-cache", ptmp, glblPathChar);

   /*
   static bool bFirstCall = 1;
   if (bFirstCall) {
      bFirstCall = 0;
      if (infoAllowed())
         pinf("using cache %s\n", szPath);
   }
   */

   return szPath;
}

int DiskCacheConfig::setPath(char *ppath)
{
   strcopy(szPath, ppath);

   char szSubName[100];
   sprintf(szSubName, "%c00-sfk-cache", glblPathChar);
   int nsublen = strlen(szSubName);

   // always force "00-sfk-cache" as subdir:
   int nlen = strlen(szPath);
   if (nlen >= nsublen && !strcmp(szPath+nlen-nsublen, szSubName))
      return 0;

   if (nlen > 0 && szPath[nlen-1] == '/' ) szPath[nlen-1] = '\0';
   if (nlen > 0 && szPath[nlen-1] == '\\') szPath[nlen-1] = '\0';

   nlen = strlen(szPath);
   int nrem = (sizeof(szPath)-10)-nlen;
   if (nrem < nsublen)
      return 9+perr("cache path too long: %s", ppath);

   strcat(szPath, szSubName);

   return 0;
}

void DiskCacheConfig::setActive(bool byesno) { bactive = byesno; }
bool DiskCacheConfig::getActive( ) { return bactive; }

DiskCacheConfig glblDiskCache;

// optional query callback on first download
bool (*pGlblDiskCacheAskSave)() = 0;

void  setDiskCacheActive(bool b) { glblDiskCache.setActive(b); }
bool  getDiskCacheActive( )      { return glblDiskCache.getActive(); }
void  setDiskCachePath(char *p)  { glblDiskCache.setPath(p); }
// guarantees non-void pointer: on error, path is "".
char *getDiskCachePath( )        { return glblDiskCache.getPath(); }

CoiMap::CoiMap( )
{
   #ifdef USE_DCACHE_EXT
   nClDAlloc   = 0;
   nClDUsed    = 0;
   apClDMeta   = 0;
   apClDFifo   = 0;
   #endif // USE_DCACHE_EXT

   nClByteSize = 0;
   nClBytesMax = 0;
   nClDropped  = 0;

   reset(0, "ct");
}

CoiMap::~CoiMap( ) {
   reset(0, "dt");
}

void CoiMap::reset(bool bWithDiskCache, const char *pszFromInfo)
{__
   mtklog(("CoiMap-reset diskcache=%d from %s", bWithDiskCache, pszFromInfo));

   // delete all clFifo entries
   ListEntry *pcur  = clFifo.first();
   ListEntry *pnext = 0;
   for (;pcur; pcur=pnext)
   {
      Coi *pcoi = (Coi *)pcur->data;
      if (pcoi->refcnt() > 1) { // 1: managed only by us
         if (!bGlblEscape) {
            pinf("cannot drop cache entry with %d refs: %s (%p)\n", pcoi->refcnt(), pcoi->name(), pcoi);
         }
      } else {
         nClByteSize -= pcoi->getUsedBytes();
         pcoi->decref();
         delete pcoi;
      }
      pnext = pcur->next();
      delete pcur;
   }
   // is now ready for flat reset
   clFifo.reset();

   extern int (*pGlblSFKStatusCallBack)(int nMsgType, char *pmsg);

   KeyMap::reset();
   if (nClByteSize != 0) {
      // TODO: so far, the bytesize is just an approximate value.
      nClByteSize = 0;
   }

   #ifdef USE_DCACHE_EXT
   if (bWithDiskCache)
   {
      mtklog(("dcache.release alloc=%d used=%d",nClDAlloc,nClDUsed));
      if (apClDMeta) { delete [] apClDMeta; apClDMeta = 0; }
      if (apClDFifo) { delete [] apClDFifo; apClDFifo = 0; }
      nClDAlloc = 0;
      nClDUsed  = 0;
   }
   #endif // USE_DCACHE_EXT
}

int CoiMap::tellByteSizeChange(Coi *pcoi, num nOldSize, num nNewSize)
{
   if (!pcoi || !pcoi->bClInCache) return 9;

   nClByteSize += (nNewSize - nOldSize);

   if (nClByteSize > nClBytesMax)
       nClBytesMax = nClByteSize;

   return 0;
}

num CoiMap::byteSize(bool bCalcFromList)
{
   if (!bCalcFromList)
      return nClByteSize;

   // expensive recalc:
   int nelem  = size();
   num  nbytes = 0;
   for (int i=0; i<nelem; i++) {
      Coi *pcoi = (Coi*)iget(i);
      if (!pcoi) { perr("int. #108281302"); break; }
      if (pcoi->hasData())
         nbytes += pcoi->getUsedBytes();
   }
   return nbytes;
}

num CoiMap::bytesMax( )     { return nClBytesMax; }
num CoiMap::filesDropped( ) { return nClDropped; }

int CoiMap::put(char *pkey, Coi *pcoi, const char *pTraceFrom, int nmode)
{__
   if (!pkey) return 9+perr("int. #208290634");
   if (!pcoi) return 9+perr("int. #208290635");

   bool bSkipDiskCache = (nmode & 1) ? 1 : 0;

   mtklog(("cache: put %s from=%s coiname=%s", pkey, pTraceFrom, pcoi->name()));

   if (pcoi->bClInCache)
      return 1+pwarn("cache-put twice, ignoring: %s", pkey);

   num nAddSize = pcoi->getUsedBytes();

   // drop cache entries due to memlimit?
   while (nClByteSize + nAddSize > nGlblMemLimit)
   {
      // drop first entry with refcnt == 1.
      // scan first 1000 cache entries, then bail out.
      // list is CHANGED below, therefore have always to re-run from start.
      ListEntry *plx = clFifo.first();
      int nbail = 1000;
      for (; plx; plx = plx->next()) {
         Coi *pxcoi = (Coi*)plx->data;
         if (pxcoi && pxcoi->refcnt() <= 1) break;
         if (nbail-- <= 0) break;
      }

      if (!plx) {
         bool btold = 0;
         if (!btold) { btold = 1;
            pwarn("cache overflow (%d), try to increase -memlimit (current=%d).\n", (int)size(), (int)(nGlblMemLimit/1000000));
         }
      }
      else
      {
         Coi *pxcoi = (Coi*)plx->data;
         if (!pxcoi) break;
 
         char *pxkey = pxcoi->name();
         // int nkeylen = strlen(pxkey);
 
         if (!quietMode())
         {
            char szAddInfo[100];
            snprintf(szAddInfo, sizeof(szAddInfo)-10, "%u files %u mb", size(), (uint)(nClByteSize/1000000UL));
            info.setAddInfoWidth(strlen(szAddInfo));
            info.setStatus("free", pxkey, szAddInfo, eKeepAdd);
            // pinf("cache-drop: %d %d %.50s\n", ndropmb, ncachmb, pxinfo);
         }
 
         mtklog(("cache-drop: %s", pxkey));
 
         // successful remove will DELETE pxcoi AND pxkey!
         if (remove(pxkey))
            { perr("int. #258282159 on cache drop"); break; }

         nClDropped++;
      }
   }

   // overwriting an existing cache entry?
   if (KeyMap::isset(pkey)) {
      // CoiMap::remove takes care of references
      mtklog(("cache-drop: %s (existing)", pkey));
      remove(pkey);
      nClDropped++;
   }

   int nrc = KeyMap::put(pkey, pcoi);
   if (!nrc) {
      // base put ok, add also in fifo list
      ListEntry *pentry = new ListEntry();
      pentry->data = pcoi;
      clFifo.add(pentry);
   }

   // INCREMENT THE REFCNT.
   pcoi->incref("cput");

   pcoi->bClInCache = 1;

   nClByteSize += pcoi->getUsedBytes();

   if (nClByteSize > nClBytesMax)
       nClBytesMax = nClByteSize;

   #ifdef USE_DCACHE
   // cache only net files
   if (   !bSkipDiskCache && pcoi->isNet()
       && pcoi->isKnownArc() && !pcoi->isZipSubEntry()
       && pcoi->hasContent()
      )
   {
      // no matter if active or not, if this is set
      if (pGlblDiskCacheAskSave)
      {
         // then ask back on first download
         bool nrc = pGlblDiskCacheAskSave();
         glblDiskCache.setActive(nrc);
         pGlblDiskCacheAskSave = 0;
      }

      if (glblDiskCache.getActive()) {
         putDiskCache(pkey, pcoi, pTraceFrom);
      } else {
         mtklog(("cache-put : no disk caching (active=%d cont=%d)", glblDiskCache.bactive, pcoi->hasContent()));
      }
   }
   else
   {
      mtklog(("cache-put: skip disk, bskip=%d isnet=%d isarc=%d issub=%d cont=%d",
         bSkipDiskCache, pcoi->isNet(), pcoi->isKnownArc(),
         pcoi->isZipSubEntry(), pcoi->hasContent()
         ));
   }
   #endif // USE_DCACHE

   return nrc;
}

int md5FromString(char *psz, num &rsumhi, num &rsumlo)
{
   SFKMD5 md5;
   md5.update((uchar*)psz, strlen(psz));
   uchar *pdig = md5.digest();
   num nlo=0, nhi=0;
   memcpy(&nhi, pdig+0, 8);
   memcpy(&nlo, pdig+8, 8);
   rsumhi = nhi;
   rsumlo = nlo;
   return 0;
}

#ifdef USE_DCACHE
char *Coi::cacheName(char *pnamein, char *pbuf, int nmaxbuf, int *pDirPartLen)
{__
   char *prel = pnamein;

   // only cache net files to disk.
   cchar *pprot = "";
   if (strBegins(prel, "http://"))
      { pprot="http"; prel += strlen("http://"); }
   else
   if (strBegins(prel, "https://"))
      { pprot="https"; prel += strlen("https://"); }
   else
   if (strBegins(prel, "ftp://"))
      { pprot="ftp"; prel += strlen("ftp://"); }
   else {
      // local file: do NOT cache to disk
      return 0;
   }

   //  in: http://foo.com/sub/dir/get.php?a=1&b=5&sess=c9352ff7
   // out: c:\temp\00-sfk-cache\foo.com\subdirget.phpa1b5sessc9352ff7
   char *pCacheDir = glblDiskCache.getPath();
   if (!pCacheDir) return 0; // error was told

   if (nmaxbuf < 100) {
      static bool btold2 = 0;
      if (!btold2) { btold2=1; pwarn("cannot cache to disk: name buffer too small.\n"); }
      return 0;
   }

   nmaxbuf -= 10; // safety

   // add: "c:\temp\00-sfk-cache" and "\"
   mystrcopy(pbuf, pCacheDir, nmaxbuf);
   strcat(pbuf, glblPathStr);

   // also add protocol to allow rebuild of url
   strcat(pbuf, pprot);
   strcat(pbuf, glblPathStr);

   char *pdst = pbuf + strlen(pbuf);
   int  nrem = nmaxbuf - strlen(pbuf);

   if (pDirPartLen) *pDirPartLen = strlen(pbuf);

   // prel: foo.com/sub/dir/get.php?a=1&b=5&sess=c9352ff7
   char *prel2 = strchr(prel, '/');
   if (!prel2) return 0; // wrong name format
   int ndomlen = prel2 - prel;
   prel2++;

   // prel2: sub/dir/get.php?a=1&b=5&sess=c9352ff7
   // add: "foo.com\"
   if (ndomlen >= nrem) {
      pwarn("cannot cache to disk, name too long: %s\n", pnamein);
      return 0;
   }
   memcpy(pdst, prel, ndomlen);
   pdst += ndomlen; nrem -= ndomlen;
   *pdst++ = glblPathChar; nrem--;
   *pdst = '\0';

   // convert and add rest of url.
   // be very careful on name reductions,
   // to avoid mixup with archive sub entries
   // when retrieving stuff from the cache later.

   // http: $-_.+!*'(),
   char szBuf[10];
   char *psrc = prel2;
   while (*psrc && (nrem > 0))
   {
      char c = *psrc++;
      if (isalnum(c))
         { *pdst++ = c; nrem--; continue; }

      #ifdef REDUCE_CACHE_FILE_NAMES
      // if finding ".zip?parm=..." then stop here.
      // but make sure ".zip\\" or ".zip//" is never stripped,
      // to avoid mixup with sub entries later.
      if (c == '.')
      {
         // scan for the longest .tar.gz etc. extension.
         // arcExtList is sorted by reverse length.
         for (int i=0; arcExtList[i]; i++)
         {
            if (!mystrnicmp(psrc-1, arcExtList[i], strlen(arcExtList[i])))
            {
               // an extension match is found, but how does it continue?
               int nextlen = strlen(arcExtList[i]);
               if (nextlen < 1) break; // safety
               char *pcont  = psrc - 1 + nextlen;

               // never strip sub archive identifiers
               if (!strncmp(pcont, "\\\\", 2)) continue;
               if (!strncmp(pcont, "//", 2))   continue;

               memcpy(pdst, arcExtList[i], nextlen);
               pdst[nextlen] = '\0';

               // EARLY EXIT: archive extension found
               mtklog(("cache: built aext name: %s", pbuf));
               return pbuf;
            }
         }
      }
      #endif

      switch (c)
      {
         case '-': case '+': case '.': case '_':
            *pdst++ = c; nrem--; continue;

         // case '_':
         //    *pdst++ = '-'; nrem--; continue;

         // case '/':
         // case '\\':
         //    *pdst++ = '_'; nrem--; continue;
      }
      sprintf(szBuf, "%%%02X", (unsigned)c);
      *pdst++ = szBuf[0];
      *pdst++ = szBuf[1];
      *pdst++ = szBuf[2];
      nrem -= 3;
   }
   *pdst = '\0';

   if (nrem <= 0) {
      pwarn("cannot cache to disk, name too long: %s\n", pnamein);
      return 0;
   }
 
   return pbuf;
}

// make sure coi has cached data before calling this
int CoiMap::putDiskCache(char *pkey, Coi *pcoi, const char *pTraceFrom)
{__
   char szCacheName[SFK_MAX_PATH+10];

   uchar *pdata = pcoi->data().src.data;
   num    nsize = pcoi->data().src.size;

   if (!pdata) return 9+perr("int. 35291439");

   // the coi may have been redirected. in that case,
   // the dstname is different from the srcname.
   char *psrcname = pkey;
   char *pdstname = pcoi->name();

   int  nCacheDirLen = 0;
   char *pszCacheName = Coi::cacheName(pdstname, szCacheName, SFK_MAX_PATH, &nCacheDirLen);
   if (!pszCacheName) return 0;

   if (createOutDirTree(pszCacheName))
      return 5+pwarn("cannot create caching dir for: %s", pszCacheName);

   if (infoAllowed())
      pinf("saving %s\n", pszCacheName);

   FILE *fout = fopen(pszCacheName, "wb");
   if (!fout)
      return 5+pwarn("cannot write cache file: %s", pszCacheName);

   mtklog(("cache: disk: write pdata=%p size=%d", pdata, (int)nsize));

   if (myfwrite(pdata, nsize, fout) != nsize) {
      fclose(fout);
      return 5+pwarn("failed to write cache file, probably disk full: %s", pszCacheName);
   }

   fclose(fout);

   // if the coi was redirected, also store the source info.
   if (strcmp(psrcname, pdstname))
   {
      pszCacheName = Coi::cacheName(psrcname, szCacheName, SFK_MAX_PATH, &nCacheDirLen);
      if (!pszCacheName) return 0;
 
      if (createOutDirTree(pszCacheName))
         return 5+pwarn("cannot create caching dir for: %s", pszCacheName);
 
      if (infoAllowed())
         pinf("saving %s\n", pszCacheName);
 
      FILE *fout = fopen(pszCacheName, "wb");
      if (!fout)
         return 5+pwarn("cannot write cache meta file: %s", pszCacheName);

      fprintf(fout, "[sfk-cache-redirect]\n%s\n", pdstname);
 
      fclose(fout);
   }

   mtklog(("cache: put on disk: %s (%s)", pkey, pszCacheName));

   return 0;
}
#endif // USE_DCACHE

#ifdef USE_DCACHE_EXT
// rc =0:found_and_index_set
// rc <0:insert_before_index
// rc >0:insert_after_index
int CoiMap::bfindDMeta(num nsumlo,num nsumhi,int &rindex)
{__
   // binary search for key, or insert position
   uint nbot=0,ndist=0,nhalf=0,imid=0;
   uint ntop=nClArrayUsed; // exclusive

   num   ntmphi=0,ntmplo=0;

   int    ncmp=-1;   // if empty, insert before index 0

   while (1)
   {
      if (nbot > ntop) // shouldn't happen
         { perr("int. 187281850"); ncmp=-1; break; }

      ndist = ntop - nbot;
      // mtklog(("dist %d bot %d top %d",ndist,nbot,ntop));
      if (ndist == 0) break; // nothing left
      nhalf = ndist >> 1;
      imid  = nbot + nhalf;

      ntmphi= apClDMeta[imid].sumhi;
      ntmplo= apClDMeta[imid].sumlo;

      // 128 bit comparison
      if (nsumhi < ntmphi) ncmp = -1;
      else
      if (nsumhi > ntmphi) ncmp =  1;
      else
      if (nsumlo < ntmplo) ncmp = -1;
      else
      if (nsumlo > ntmplo) ncmp =  1;
      else
         ncmp = 0;

      if (ncmp < 0) {
         // select lower half, if any
         // mtklog((" take lower %xh %xh %d",nval,ntmp,imid));
         if (ntop == imid) break; // safety
         ntop = imid;
      }
      else
      if (ncmp > 0) {
         // select upper half, if any
         // mtklog((" take upper %xh %xh %d",nval,ntmp,imid));
         if (nbot == imid+1) break; // required
         nbot = imid+1;
      } else {
         // straight match
         mtklog(("%d = indexof(%xh) used=%u",imid,(uint)nsumhi,nClDAlloc));
         break; // found
      }
   }

   rindex = imid;
   return ncmp;
}

int CoiMap::putDiskCache(char *pkey, Coi *pcoi)
{__
   int nfree = nClDAlloc - nClDUsed;
   if (nfree < 10) {
      // expand arrays: alloc new
      int nAllocNew = nClDAlloc + (nClDAlloc ? nClDAlloc : 100);
      DMetaEntry *pdnew = new DMetaEntry[nAllocNew+10];
      DFifoEntry *pfnew = new DFifoEntry[nAllocNew+10];
      if (nClDUsed) {
         memcpy(pdnew, apClDMeta, sizeof(DMetaEntry)*nClDUsed);
         memcpy(pfnew, apClDFifo, sizeof(DFifoEntry)*nClDUsed);
      }
      // then free old and swap
      delete [] apClDMeta;
      delete [] apClDFifo;
      apClDMeta = pdnew;
      apClDFifo = pfnew;
      nClDAlloc = nAllocNew;
      nfree = nClDAlloc - nClDUsed;
   }
   // put basic infos into (mem) cached meta data
   num nsumlo=0,nsumhi=0;
   md5FromString(pkey,nsumlo,nsumhi);

   // find insert position
   int nindex = 0;
   int nrc = bfindDMeta(nsumlo,nsumhi,nindex);
   if (!nrc) return 1;  // already in cache

   if (nrc < 0) {
      // insert before index
   } else {
      // insert after index
      nindex++;
   }
   int ntomove = nClDUsed - nindex;
   if (ntomove > 0)
      memmove(&apClDMeta[nindex+1], &apClDMeta[nindex+0],
         sizeof(DMetaEntry) * ntomove);
   // now, set at index
   DMetaEntry *pdmet = &apClDMeta[nindex];
   pdmet->sumhi = nsumhi;
   pdmet->sumlo = nsumlo;
   pdmet->size  = pcoi->getSize();
   pdmet->time  = pcoi->getTime();
   // append key entry to fifo
   DFifoEntry *pdfif = &apClDFifo[nClDUsed];
   pdfif->sumhi = nsumhi;
   pdfif->sumlo = nsumlo;
   // finally, count new entry
   nClDUsed++;
   mtklog("dcache.insert at %d of %d: sumlo=%s key=%s", nindex, nClDUsed,
      numtohex(nsumlo), pkey);
   return 0;
}
#endif // USE_DCACHE_EXT

// caller MUST release object after use!
Coi *CoiMap::get(char *pkey)
{__
   Coi *pcoi = (Coi*)KeyMap::get(pkey);
   if (pcoi) {
      pcoi->incref("cget");
      mtklog(("cache: %p = cache.get( %s )",pcoi,pkey));
      return pcoi;
   }

   #ifdef USE_DCACHE

   // disk cache is yet used only for net files
   if (   !strBegins(pkey, "http://")
       && !strBegins(pkey, "https://")
       && !strBegins(pkey, "ftp://"))
      return 0;

   // not yet in mem cache, but maybe in disk cache?
   if (!glblDiskCache.bactive) return 0;

   mtklog(("cache: cache.get( %s ) begin",pkey));

   char szCachePathBuf[SFK_MAX_PATH+10];
   char szCacheReDirBuf[SFK_MAX_PATH+10];
   char *pszDiskCacheFile = Coi::cacheName(pkey, szCachePathBuf, SFK_MAX_PATH, 0);

   if (!pszDiskCacheFile) {
      mtklog(("cache: no disk name: %s",pkey));
      return 0;
   }

   num nFileSize = getFileSize(pszDiskCacheFile);
   if (nFileSize < 0) {
      return 0;
   }

   // reject cache files beyond the memory limit
   if (nFileSize > nGlblMemLimit) {
      pinf("cache file too large, cannot load: %s\n", pszDiskCacheFile);
      return 0;
   }

   if (infoAllowed())
      pinf("using cache file %s\n", pszDiskCacheFile);

   // exists in disk cache: create a memory coi in mem cache.
   uchar *pdata = loadBinaryFile(pszDiskCacheFile, nFileSize);
   if (!pdata) {
      pinf("failed to load cache file: %s\n", pszDiskCacheFile);
      return 0;
   }

   mtklog(("cache: loaded, size=%d: %s",(int)nFileSize,pszDiskCacheFile));

   if (strBegins((char*)pdata, "[sfk-cache-redirect]"))
   {
      // have to load another file with actual content
      char *pdstname = (char*)pdata + strlen("[sfk-cache-redirect]");
      while (*pdstname && (*pdstname != '\n')) pdstname++;
      if (*pdstname) pdstname++;
      char *psz2 = pdstname;
      while (*psz2 && *psz2 != '\r' && *psz2 != '\n') psz2++;
      *psz2 = '\0';

      strcopy(szCacheReDirBuf, pdstname);
      pdstname = szCacheReDirBuf;

      // target name isolated, clear temporary:
      delete [] pdata; pdata = 0;
      // ptr will be reused immediately

      // now holding the virtual filename in pdstname (http://...)
      // build cache name from that.
      pszDiskCacheFile = Coi::cacheName(pdstname, szCachePathBuf, SFK_MAX_PATH, 0);
      if (!pszDiskCacheFile) {
         mtklog(("cache: no cache name for: %s", pdstname));
         return 0;
      }

      pinf("using cache file %s\n", pszDiskCacheFile);

      pdata = loadBinaryFile(pszDiskCacheFile, nFileSize);
      if (!pdata) {
         pinf("failed to load cache file: %s\n", pszDiskCacheFile);
         return 0;
      }

      mtklog(("cache: loaded redir, size=%d: %s",(int)nFileSize,pszDiskCacheFile));
   }

   // the cacheName function must make sure that keys for subentries
   // never return a cache file name pointing to the overall .zip.
   // i.e. if the name is reduced, reduction shall not strip sub infos,
   // otherwise we retrieve wrong data (and coi names) here.
   pcoi = new Coi(pszDiskCacheFile, 0);

   pcoi->setContent(pdata, nFileSize);

   mtklog(("cache: put entry: key=%s", pkey));
   mtklog(("cache: put entry: coi=%s", pcoi->name()));

   put(pkey, pcoi, "dcache", 1); // skipping disk cache write
   // TODO: what to do if memCachePut fails after diskCacheLoad?

   // put() sets the refcnt to 1.
   // but as get() was originally called,
   // we must return refcnt==2:
   pcoi->incref("cget2");

   #endif // USE_DCACHE

   return pcoi;
}

int CoiMap::remove(char *pkey)
{
   // get coi, need it for fifo search
   // do NOT use CoiMap::get() as it increments the ref!
   Coi *pcoi = (Coi*)KeyMap::get(pkey);
   if (!pcoi) return 1; // not found

   // remove map entry
   int nrc = KeyMap::remove(pkey);

   // remove from fifo list as well
   ListEntry *plx = clFifo.first();
   for (;plx; plx=plx->next())
      if (plx->data == pcoi)
         break;

   // no list entry should NOT happen
   if (plx) {
      clFifo.remove(plx);
      delete plx;
      plx = 0;
   } else {
      perr("int. 187282050");
   }

   // finally, delete the managed coi
   if (pcoi->refcnt() > 1) { // 1: managed only by us
      if (!bGlblEscape) {
         pinf("cannot drop cache entry with %d refs: %s", pcoi->refcnt(), pcoi->name());
      }
   }
   else
   {
      nClByteSize -= pcoi->getUsedBytes();

      pcoi->bClInCache = 0;

      pcoi->decref();

      delete pcoi;
   }

   return nrc;
}

int Coi::rawLoadDir(int ilevel)
{__
   if (data().bloaddirdone) {
      mtklog(("coi::loaddir: done, %d entries", data().elements().numberOfEntries()));
      return 1; // already done
   }
   data().bloaddirdone = 1;

   #ifdef SFKOFFICE
   if (isOffice(106)) return rawLoadOfficeDir();
   #endif // SFKOFFICE

   #ifdef SFKDEEPZIP
   if (cs.probefiles)
      probeFile();
   #endif // SFKDEEPZIP


   #ifdef VFILEBASE
   if (isFtp())   return rawLoadFtpDir();
   #endif // VFILEBASE

   return 9; // n/a with fsdirs
}

// caller MUST RELEASE COI after use!
Coi *Coi::rawNextFtpEntry( )
{
   if (!data().bdiropen) {
      perr("ftp nextEntry() called without openDir()");
      return 0;
   }

   Coi *psubsrc = 0;

   #ifndef DEEP_FTP
   do
   #endif
   {
      // something left to return?
      if (data().nNextElemEntry >= data().elements().numberOfEntries())
         return 0; // end of list

      // return a COPY from the internal list entries.
      psubsrc = data().elements().getEntry(data().nNextElemEntry, __LINE__);
      data().nNextElemEntry++;
   }
   #ifndef DEEP_FTP
   while (psubsrc->isTravelDir()); // for now, SKIP dir entries of ftp
   #endif

   // if there is a global cache entry machting this sub,
   Coi *pcached = glblVCache.get(psubsrc->name());

   // caller MUST RELEASE COI after use!

   if (pcached)
   {
      bool bhasptr = pcached->data().src.data ? 1 : 0; (void)bhasptr;
      mtklog(("coi::nextftpentry cache hit %d %d %s", (int)bhasptr, (int)pcached->data().src.size, pcached->name()));
      return pcached;
   }
   else
   {
      Coi *psubdst = psubsrc->copy();
      psubdst->incref("nft");
      mtklog(("coi::nextftpentry cache miss, returning copy of %s", psubdst->name()));
      return psubdst;
   }
}

void Coi::rawCloseFtpDir( ) {
   // see remarks in rawCloseZipDir
   data().bdiropen = 0;
}

int Coi::rawOpenFtpSubFile(cchar *pmode)
{
   if (!isFtp()) return 9;
   if (strcmp(pmode, "rb")) return 9;

   // get client for that base url
   if (data().getFtp(name())) return 9;

   // use supplied client, release after dir download
   FTPClient *pftp = data().pClFtp;

   // isolate hostname from url
   if (pftp->splitURL(name()))
      return 9; // error was told

   // nRelIndex is now the relative path start index.
   char *phost    = pftp->curhost();
   char *prelfile = pftp->curpath();
   int  nport    = pftp->curport();

   mtklog((" ftp open host \"%s\" file \"%s\"", phost, prelfile));

   int nrc = 0;

   for (int itry=0; itry<2; itry++)
   {
      if (pftp->loginOnDemand(phost, nport))
         return 9;

      nrc = pftp->openFile(prelfile, pmode);
      // RC  9 == general error, e.g. file not available
      // RC 10 == communication failed, connection invalid

      if (nrc < 10)
         break;

      // loginOnDemand thought the line is still valid, but it isn't.
      pinf("ftp session expired, retrying\n");
 
      // devalidate session, relogin and retry
      pftp->logout();
   }
 
   if (nrc) {
      if (nrc > 9) {
         pinf("ftp communication failed (connection closed)\n");
         pftp->logout();
      }
      data().releaseFtp();  // failed
   }
   else
      data().bfileopen = 1;

   // general releaseFtp is done after file download.

   return nrc;
}

int Coi::rawOpenHttpSubFile(cchar *pmode)
{__
   if (!isHttp()) return 9;
   if (strcmp(pmode, "rb")) return 9;

   if (data().getHttp(name())) return 9;
   HTTPClient *phttp = data().pClHttp;

   mtklog(("http-open %s",name()));

   int nrc = phttp->open(name(), pmode, this);
   // may redirect and change current coi's name!

   if (nrc) {
      if (nrc==11 && !cs.verbose)
         { }
      else
      {
         if (root(1)) pinf("[nopre] from : %s\n", root());
         if (ref(1))  pinf("[nopre] ref  : %s\n", ref());
      }
   } else {
      data().bfileopen = 1;
   }

   return nrc;
}

extern char *getxlinfo();
extern num   getxllim();

size_t Coi::rawReadFtpSubFile(void *pbufin, size_t nBufSize)
{
   if (!data().pClFtp || !data().bfileopen) {
      perr("ftp not open, cannot read: %s (%d)", name(), data().bfileopen);
      return 0;
   }

   int nread = data().pClFtp->readFile((uchar*)pbufin, (int)nBufSize);
   mtklog((" ftp read %s %u done %d", name(), (uint)nBufSize, nread));

   return (nread >= 0) ? nread : 0;
}

size_t Coi::rawReadHttpSubFile(void *pbufin, size_t nBufSize)
{__
   if (!data().pClHttp || !data().bfileopen) {
      perr("http not open, cannot read: %s (%d)", name(), data().bfileopen);
      return 0;
   }

   int nread = data().pClHttp->read((uchar*)pbufin, (int)nBufSize);
   mtklog(("http-read done=%d buf=%p max=%d", nread, pbufin, (int)nBufSize));

   return (nread >= 0) ? nread : 0;
}

void Coi::rawCloseFtpSubFile( )
{
   mtklog(("ftp-close %s", name()));

   if (!data().pClFtp) {
      mtklog(("close.ftp already done: %s",name()));
      return;
   }

   data().pClFtp->closeFile();

   // release connection ptr, without closing it:
   data().releaseFtp();

   data().bfileopen = 0;
}

void Coi::rawCloseHttpSubFile( )
{__
   mtklog(("http-close %s", name()));

   if (!hasData()) {
      mtklog(("close.http already done: %s",name()));
      return;
   }

   if (!data().pClHttp) {
      mtklog(("close.http already done: %s",name()));
      return;
   }

   // so far, coi http core can NOT reuse the same connection
   // for multiple commands, so we MUST close the socket.
   // (otherwise GET without full read must be avoided)
   data().pClHttp->close();

   // release connection ptr, without closing it:
   data().releaseHttp();

   data().bfileopen = 0;
}

int Coi::prefetch(bool bLoadNonArcBinaries, num nFogSizeLimit, num nHardSizeLimit)
{
   return preload("dvw", 0, bLoadNonArcBinaries ? 1 : 2); // dview
}

int Coi::preload(cchar *pszFromInfo, uchar **ppout, num &rsize, int iStopMode)
{
   int isubrc = preload(pszFromInfo, 0, iStopMode, 1); // internal
   if (isubrc) return isubrc;

   *ppout = data().src.data;
   rsize  = data().src.size;

   return 0;
}

// stopmode 0: load everything
// stopmode 1: load no binaries except archives
// stopmode 2: load no binaries
int Coi::preload(cchar *pszFromInfo, bool bsilent, int iStopMode, bool bfile)
{
   if (data().src.data)
      return 0; // nothing to do

   if (cs.debug)
      printf("preload.1: %s %d mode=%d file=%d %s\n", pszFromInfo, bsilent, iStopMode, bfile, name());

   #ifdef SFKPACK
   if (isOfficeSubEntry())
      return loadOfficeSubFile("preload");
   #endif // SFKPACK

   bool bIsZipTrav = isTravelZip(111,1);
   bool bIsZipSub  = isZipSubEntry();
   bool bIsZipAny  = (bIsZipTrav || bIsZipSub);

   if (isNet()) // && bIsZipTrav)
      {_ } // accept, need to cache whole file
   else
   if (bIsZipSub)
      {_ } // accept, need to cache sub entry via parent
   else
   if (!bfile) {
      // physical file: no preload
      return 0;
   }

   num  nLoadLimit   = nGlblMemLimit;
   cchar *pszLimitOpt= "-memlimit";
   num  nAllocSize   = 2000000; // 2 mb
   bool bStreamLoad  = 1;


   if (isHttp()) {
      nLoadLimit  = cs.maxwebsize;
      pszLimitOpt = "-weblimit";
   }

   int iopenrc=0,iopenrc2=0;

   if ((iopenrc = open("rb")))
   do
   {
      if (isHttp())
      {
      }

      // if (!bsilent)
      //    pwarn("cannot read: %s\n", name());

      return 9;
   }
   while (0);

   if (hasSize()) {
      nAllocSize  = getSize();
      bStreamLoad = 0;
      if (cs.debug) printf("preload.2: have=%u size=%d\n",nClHave,(int)nAllocSize);
   } else {
      if (cs.debug) printf("preload.2: have=%u alloc=%d\n",nClHave,(int)nAllocSize);
   }

   if (nAllocSize < 0) {
      close();
      return 9;
   }

   if (nAllocSize > nLoadLimit) {
      close();
      if (!bsilent) {
         pwarn("file too large, skipping: %s (%d mb)\n", name(), (int)(nAllocSize/1000000));
         if (pszLimitOpt)
            pinf("use option %s to change load limit\n", pszLimitOpt);
      }
      return 1; // block loading
   }

   uchar *pdata  = new uchar[nAllocSize+100];
   num    nused  = 0;

   if (!pdata) { // safety
      close();
      return 9+perr("out of memory (%d)\n", (int)(nAllocSize/1000000));
   }

   if (   iStopMode > 0
       && (nClHave & COI_HAVE_BINARY) == 0
      )
   {
      mtklog(("read probe"));

      int nProbeSize = mymin(1024, nAllocSize);

      memset(pdata, 0, nProbeSize);

      nused = read(pdata, nProbeSize);

      if (nused < 0) {
         close();
         return 9+perr("cannot read: %s\n", name());
      }

      pdata[nused]='\0';
      uchar *p = pdata;

      if (nused>0 && memchr(pdata, 0, nused)) {
         if (p[1]=='P' && p[2]=='N' && p[3]=='G') {
            bClWebImage=1; bClWebPNG=1;
            bClWebBinary=1; bClWebText=0;
            setBinaryFile(1);
         } else if (p[0]==0xFF && p[1]==0xD8) {
            bClWebImage=1; bClWebJpeg=1;
            bClWebBinary=1; bClWebText=0;
            setBinaryFile(1);
         } else {
            bClBinary = 1;
         }
      } else {
         bClBinary = 0;
      }
      nClHave |= COI_HAVE_BINARY;

      bClWebBinary = bClBinary;
      bClWebText   = bClWebBinary ? 0 : 1;
   }

   bool bIsKnownBinary = ((nClHave & COI_HAVE_BINARY) && bClBinary) ? 1 : 0;
   bool bIsKnownArc    = ((nClHave & COI_HAVE_ARC   ) && bClArc   ) ? 1 : 0;

   switch (iStopMode)
   {
      case 0: // load everything
         break;

      case 1: // load no binaries except archives
         if (bIsKnownBinary && !bIsKnownArc)
         {
            if (cs.debug)
               printf("preload stop: binary and no archive\n");
            close();
            return 2; // skip download and caching as it's not text, and no archive
         }
         break;

      case 2: // load no binaries
         if (bIsKnownBinary)
         {
            if (cs.debug)
               printf("preload stop: binary\n");
            close();
            return 3; // skip download and caching as it's not text, and no archive
         }
         break;
   }

   if (cs.debug) printf("preload.4: probe=%d\n",(int)nused);
 
   while (1)
   {
      num nrem = nAllocSize - nused;

      if (bStreamLoad)
      {
         // expand buffer by next stream data
         if (nrem < nAllocSize / 4)
         {
            num nAllocSize2 = nAllocSize * 2;
            uchar *ptmp = new uchar[nAllocSize2+100];
            memcpy(ptmp, pdata, nused);
            delete [] pdata;
            pdata  = ptmp;

            nAllocSize = nAllocSize2;
            nrem       = nAllocSize - nused;
         }
      }
      else if (nused >= nAllocSize)
      {
         break;
      }

      num nread = read(pdata+nused, nrem);

      // if (cs.debug) printf("preload.5: read %d\n",(int)nread);

      if (nread <= 0)
         break;

      nused += nread;
   }

   if (bStreamLoad)
   {
      // how much is left empty in read cache?
      num nrem = nAllocSize - nused;
      if (nrem > 1000)
      {
         // adapt to the size really used
         num nAllocSize2 = nused + 100;
         uchar *ptmp = new uchar[nAllocSize2];
         memcpy(ptmp, pdata, nused);
         delete [] pdata;
         pdata  = ptmp;

         nAllocSize = nAllocSize2;
      }
   }

   close();

   pdata[nused] = '\0'; // is guaranteed

   if (cs.debug) printf("preload.6: data %p used=%d\n",pdata,(int)nused);
 
   setContent(pdata, nused, nClMTime);

   return 0;
}

int Coi::provideInput(cchar *pszFromInfo, bool bsilent)
{
   return preload(pszFromInfo, bsilent, 1); // provideInput
}

Coi *Coi::getElementByAbsName(char *pabsname)
{__
   if (!data().elements().numberOfEntries())
      if (rawLoadDir() >= 5)
         return 0;

   // check absname, if it matches ourselves at all
   if (!striBegins(pabsname, name())) {
      // should NOT happen
      pwarn("cannot get element, name mismatch: %s / %s", pabsname, name());
      return 0;
   }

   Coi *psub = 0;
   for (int i=0; i<data().elements().numberOfEntries(); i++)
   {
      psub = data().elements().getEntry(i, __LINE__);
      if (!strcmp(psub->name(), pabsname)) break;
   }

   return psub;
}

bool Coi::isFtp() {
   #ifdef VFILENET
   if (nGlblCoiConfig & COI_CAPABILITY_NET)
      return strBegins(name(), "ftp://");
   #endif // VFILENET
   return 0;
}

bool Coi::isHttp(char *pszOptURL) {
   #ifdef VFILENET
   char *psz = pszOptURL ? pszOptURL : name();
   if (nGlblCoiConfig & COI_CAPABILITY_NET) {
      if (strBegins(psz, "http://"))
         return 1;
      if (strBegins(psz, "https://"))
         return 1;
   }
   #endif // VFILENET
   return 0;
}

bool Coi::isNet() {
   return isHttp() || isFtp();
}

bool Coi::isVirtual(bool bWithRootZips)
{
   if (isNet() || isZipSubEntry())     return 1;
   if (bWithRootZips && isTravelZip(109)) return 1;
   return 0;
}

bool Coi::rawIsFtpDir()
{
   if (!isFtp()) return 0;

   // try to detect by name
   char *pnam = name();
   int nlen  = strlen(pnam);
   if (nlen > 0 && pnam[nlen-1] == '/')
      return 1;

   // but also use the coi flag
   return bClDir;
}

int Coi::readWebHead( )
{
   if (!isHttp())
      return 9;

   if (data().getHttp(name()))
      return 9;

   HTTPClient *phttp = data().pClHttp;

   if (phttp->getFileHead(name(), this, "head"))
      return 9+perr("cannot get headers: %s", name());

   return 0;
}

// TODO: rework error rc handling?
bool Coi::rawIsHttpDir(int ilevel)
{__

   #ifdef SFKINT
   pwarn("isdir: wrong http call sequence.\n");
   #endif

   return 0;
}

int Coi::rawLoadFtpDir( )
{
   if (!isFtp()) return 9;

   // get client for that base url
   if (data().getFtp(name())) return 9;

   // use supplied client, release after dir download
   FTPClient *pftp = data().pClFtp;
   if (pftp->splitURL(name())) {
      data().releaseFtp();
      return 9; // error was told
   }
   // nRelIndex is now the relative path start index.
   char *phost = pftp->curhost();
   char *ppath = pftp->curpath();
   int  nport = pftp->curport();

   mtklog((" ftp open host \"%s\" path \"%s\"", phost, ppath));

   if (pftp->loginOnDemand(phost, nport)) {
      data().releaseFtp();
      return 9+perr("ftp login failed: %s", phost);
   }

   // let the ftp client fill our elements list.
   // it must prefix any name by our name.
   CoiTable *plist = &data().elements();
   plist->resetEntries(); // if any
   if (pftp->list(ppath, &plist, name())) {
      data().releaseFtp();
      return 9+perr("ftp list failed: %s", phost);
   }

   // list downloaded: do NOT close connection
   //   pftp->logout();
   // as it may be used for subsequent downloads

   // start from zip entry 0
   data().nNextElemEntry = 0;

   // current I/O job done: release the client,
   // meaning the refcnt is dec'ed, but NO logout.
   data().releaseFtp();

   return 0;
}

num Coi::getUsedBytes()
{
   num nsize = sizeof(*this);

   if (pszClName)   nsize += strlen(pszClName)+1;
   if (pszClRoot)   nsize += strlen(pszClRoot)+1;
   if (pszClRef)    nsize += strlen(pszClRef)+1;
   if (pszClExtStr) nsize += strlen(pszClExtStr)+1;

   if (hasData()) {
      nsize += sizeof(CoiData);
      CoiData *p = pdata;
      nsize += p->src.size;
      if (p->prelsubname)  nsize += strlen(p->prelsubname)+1;
      if (p->pdirpat    )  nsize += strlen(p->pdirpat)+1;
   }

   // NOT considered, as they are volatile and may have
   // different values on cache put and remove:
   //    rbuf.data, pzip

   return nsize;
}

#endif // VFILEBASE



bool getistr(char *psz, int idigits, int ifrom, int ito, int &rout)
{
   int r=0;
   for (int i=0; i<idigits; i++)
   {
      char c = *psz++;
      if (!isdigit(c))
         return 0;
      r = r * 10 + (c - '0');
   }
   if (r < ifrom) return 0;
   if (r > ito) return 0;
   rout = r;
   return 1;
}

bool matchdate(char *pszsrcio, cchar *pszmask, int *adate, bool breorder=0)
{
   char *pszsrc = pszsrcio;

   int Y=0,M=0,D=0,h=0,m=0,s=0;
   int i=0,istate=0;

   int ireqdigits = 0;
   for (char *psz=(char*)pszmask; *psz; psz++)
      switch (*psz) {
         case 'Y': ireqdigits += 4; break;
         case 'M': ireqdigits += 2; break;
         case 'D': ireqdigits += 2; break;
         case 'h': ireqdigits += 2; break;
         case 'm': ireqdigits += 2; break;
         case 's': ireqdigits += 2; break;
      }

   int ihavedigits = 0;
   for (char *psz=pszsrc; *psz; psz++)
      if (isdigit(*psz))
         ihavedigits++;
      else
         break;

   if (ihavedigits < ireqdigits)
      return 0;

   while (*pszmask && *pszsrc)
   {
      switch (*pszmask)
      {
         case 'Y':
            if (!getistr(pszsrc,4,1970,3000,Y)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[0] = Y;
            pszmask++;
            pszsrc += 4;
            istate=1;
            continue;
         case 'M':
            if (!getistr(pszsrc,2,01,12,M)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[1] = M;
            pszmask++;
            pszsrc += 2;
            istate=1;
            continue;
         case 'D':
            if (!getistr(pszsrc,2,01,31,D)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[2] = D;
            pszmask++;
            pszsrc += 2;
            istate=1;
            continue;
         case 'h':
            if (!getistr(pszsrc,2,00,23,h)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[3] = h;
            pszmask++;
            pszsrc += 2;
            istate=1;
            continue;
         case 'm':
            if (!getistr(pszsrc,2,00,59,m)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[4] = m;
            pszmask++;
            pszsrc += 2;
            istate=1;
            continue;
         case 's':
            if (!getistr(pszsrc,2,00,59,s)) {
               if (istate) return 0;
               pszsrc++;
               continue;
            }
            adate[5] = s;
            pszmask++;
            pszsrc += 2;
            istate=1;
            continue;
      }
   }

   if (!*pszmask)
   {
      if (breorder==1 && (pszsrc-pszsrcio)==8)
      {
         // rebuild date in order: YMD
         pszsrcio[0] = ((adate[0] / 1000) % 10) + '0';
         pszsrcio[1] = ((adate[0] /  100) % 10) + '0';
         pszsrcio[2] = ((adate[0] /   10) % 10) + '0';
         pszsrcio[3] = ((adate[0] /    1) % 10) + '0';
         pszsrcio[4] = ((adate[1] /   10) % 10) + '0';
         pszsrcio[5] = ((adate[1] /    1) % 10) + '0';
         pszsrcio[6] = ((adate[2] /   10) % 10) + '0';
         pszsrcio[7] = ((adate[2] /    1) % 10) + '0';
      }
      return 1;
   }

   return 0;
}

#ifdef SFK_W64
char *dataAsTraceQ(ushort *pAnyData)
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

      *pszDstCur++ = '?';
   }
 
   *pszDstCur = '\0';
 
   return pszBuf;
}

int execFixFile(ushort *ain, sfkfinddata64_t *pdata)
{
   cs.files++;

   // derive all possibly needed data

   ushort apure[1024];  // for dewide, rewide
   char   szpure[1024]; // same in ascii
   int    adate[20];    // for setftime
   uchar  szcp[50];

   int isrc=0,idst=0,iuni=0;
   int msrc=1000,mdst=1000;
   mclear(adate);

   while (ain[isrc]!=0 && isrc<msrc && idst<mdst)
   {
      ushort uc = ain[isrc++];
      if (uc >= 0x100U)
      {
         iuni++;

         if (cs.dewide)
            continue;

         if (cs.rewide)
         {
            sprintf((char*)szcp, "{%04x}", uc);
            for (int i=0; szcp[i]!=0 && idst<mdst; i++)
            {
               apure[idst] = szcp[i];
               szpure[idst] = szcp[i];
               idst++;
            }
            continue;
         }

         apure[idst] = uc;
         szpure[idst] = '?';
         idst++;
         continue;
      }
      apure[idst] = uc;
      szpure[idst] = (char)uc;
      idst++;
   }
   apure[idst] = 0;
   szpure[idst] = '\0';

   /*
      scan for date, time
      -------------------
      01312015    MDY   201501   hms
      20150131    YMD   2015     hm

      01172015-0737  MDYhm
      20150131201501 YMDhms
   */
   bool bdate=0,btime=0,btsdiff=0;
   char *psz = strrchr(szpure, glblPathChar);
   if (!psz)
         psz = szpure;
   while (*psz)
   {
      if (!bdate) {
         if (matchdate(psz, "MDY", adate, cs.setndate)) {
            if (cs.setndate) {
               // write back possible reordered date
               int ioff = psz-szpure;
               for (int i=0; i<8; i++) {
                  if (apure[ioff+i] != ((ushort)psz[i]&0xFFU))
                     btsdiff = 1;
                  apure[ioff+i] = psz[i];
               }
            }
            bdate=1;
            psz += 8;
            continue;
         }
         if (matchdate(psz, "YMD", adate))
            { bdate=1; psz += 8; continue; }
         psz++;
         continue;
      }
      if (!btime) {
         if (matchdate(psz, "hms", adate))
            { btime=1; psz += 6; continue; }
         if (matchdate(psz, "hm", adate))
            { btime=1; psz += 4; continue; }
         psz++;
         continue;
      }
      break;
   }

   bool bNameDiffers = memcmp(ain, apure, (isrc+1)*2) ? 1 : 0;

   mytime_t now = mytime(NULL);
   struct tm *tm = 0;
   tm = mylocaltime(&now); // safe
   if (tm == 0) return 9+perr("cannot get time"); // safety

   tm->tm_isdst = -1;
   tm->tm_year = adate[0] - 1900;
   tm->tm_mon  = adate[1] - 1;
   tm->tm_mday = adate[2];
   tm->tm_hour = adate[3];
   tm->tm_min  = adate[4];
   tm->tm_sec  = adate[5];

   mytime_t nTime = mymktime(tm);
   num nTime2 = (num)nTime;

   bool bTimeDiffers = 0;

   if (nTime2 > 0 && nTime2 != pdata->time_write)
      bTimeDiffers = 1;

   // ignore, list, change?
   bool bChgName=0, bChgTime=0;
 
   if ((cs.dewide || cs.rewide) && bNameDiffers)
      bChgName = 1;

   if (cs.setftime && bTimeDiffers)
      bChgTime = 1;

   if (btsdiff)
      bChgName = 1;

   if (!bChgName && !bChgTime)
      return 0;

   printx("$FROM:<def> <time>%s<def> %s\n",
      pdata->time_write > 0 ? timeAsString(pdata->time_write,1) : "[invalid time]",
      dataAsTraceQ(ain));

   // printx("$DIFF:<def> name=%d time=%d %llu %llu\n",bChgName,bChgTime,pdata->time_write,nTime2);
   num nTime3 = 0;
   if (pdata->time_write > 0)
      nTime3 = pdata->time_write; // display default: current file time
   if (bChgTime != 0 && nTime2 > 0)
      nTime3 = nTime2;
   cchar *ptimecol = (bChgTime != 0 && nTime2 > 0) ? "<rep>":"<time>";
   cchar *pnamecol = bChgName ? "<rep>":"";
   printx("$  TO:<def> %s%s<def> %s%s<def>\n", ptimecol, timeAsString(nTime3,1), // sfk1883
      pnamecol, dataAsTraceW(apure));

   if (idst < 1) {
      pwarn("... cannot change, name would be empty: %s\n", dataAsTraceW(ain));
      return 0;
   }

   if (cs.yes)
   {
      do
      {
         if (bChgName && _wrename((const wchar_t *)ain, (const wchar_t *)apure))
         {
            perr("... rename failed: %s\n", dataAsTraceW(ain));
            break;
         }

         bool berr = 0;

         if (bChgTime != 0 && nTime2 > 0)
         {
            HANDLE hDst = CreateFileW(
               (const wchar_t *)apure,
               FILE_WRITE_ATTRIBUTES,
               0,    // share
               0,    // security
               OPEN_EXISTING,
               FILE_ATTRIBUTE_NORMAL,
               0     // template file
               );
            if (hDst == INVALID_HANDLE_VALUE)
               { perr("... set filetime failed (1)\n"); break; }

            FILETIME nDstMTime, nDstCTime;
            FILETIME *pMTime=0, *pCTime=0;
 
            if (!makeWinFileTime(nTime2, nDstMTime)) // fixfile
               pMTime = &nDstMTime;
 
            // if (nClCTime > 0)
            // {
            //    if (!makeWinFileTime(nClCTime, nDstCTime))
            //       pCTime = &nDstCTime;
            // }

            if (pMTime || pCTime)
               if (!SetFileTime(hDst, pCTime, 0, pMTime))
                  { perr("... set filetime failed (2)\n"); berr=1; }

            CloseHandle(hDst);
         }

         if (!berr)
            cs.filesChg++; // fixfile
      }
      while (0);
   }
   else
   {
      cs.filesChg++; // fixfile
   }

   return 0;
}
#endif



#ifndef USE_SFK_BASE

static unsigned char webdemo_abRawBlock[1358] = {
80,75,3,4,10,0,0,0,0,0,170,162,60,73,0,0,0,0,0,0,0,0,0,0,0,0,8,0,17,0,119,101,
98,100,101,109,111,47,85,84,13,0,7,32,10,236,87,224,235,234,87,250,181,236,87,80,75,3,4,20,0,0,0,8,
0,170,162,60,73,168,198,156,182,122,0,0,0,144,0,0,0,18,0,17,0,119,101,98,100,101,109,111,47,105,110,100,
101,120,46,104,116,109,108,85,84,13,0,7,32,10,236,87,224,235,234,87,19,182,236,87,53,142,193,10,194,48,16,68,
239,133,254,195,208,15,104,193,115,92,16,61,42,8,30,244,154,198,173,27,76,154,146,236,65,255,222,84,232,101,46,243,
120,51,70,52,6,50,99,122,126,169,109,140,236,232,230,227,18,24,119,30,113,112,142,75,193,137,99,194,213,190,216,12,
181,175,212,178,134,133,100,158,246,157,75,179,242,172,165,255,196,208,209,49,120,247,134,112,174,176,37,76,41,67,133,241,
184,156,177,129,8,190,104,191,105,134,255,116,53,175,63,218,230,7,80,75,3,4,20,0,0,0,8,0,170,162,60,73,
158,225,250,98,89,0,0,0,222,0,0,0,20,0,17,0,119,101,98,100,101,109,111,47,99,111,110,116,101,110,116,115,
46,120,109,108,85,84,13,0,7,32,10,236,87,224,235,234,87,27,182,236,87,179,41,202,47,47,182,227,229,82,176,73,
78,44,73,77,207,47,170,4,113,20,108,50,83,236,12,13,12,109,244,129,52,152,159,151,152,155,106,231,86,84,154,89,
82,108,163,15,230,128,244,232,35,107,194,102,130,17,154,9,97,169,233,169,37,137,73,57,169,36,153,98,140,102,138,83,
106,89,106,81,98,58,78,67,108,244,161,190,2,0,80,75,3,4,20,0,0,0,8,0,170,162,60,73,169,45,180,0,
91,0,0,0,13,1,0,0,28,0,17,0,119,101,98,100,101,109,111,47,112,114,111,100,117,99,116,95,108,105,115,116,
95,49,48,49,46,120,109,108,85,84,13,0,7,32,10,236,87,224,235,234,87,37,182,236,87,179,41,202,47,47,182,227,
229,82,80,80,176,1,50,237,108,242,18,115,83,237,28,11,10,114,82,139,109,244,193,28,176,36,24,216,20,20,101,38,
167,218,25,233,25,216,232,67,152,54,250,32,77,232,218,157,18,243,128,16,167,126,67,61,83,252,250,189,51,203,51,241,
232,54,199,175,219,189,40,177,0,159,227,141,209,181,131,105,80,32,0,0,80,75,3,4,20,0,0,0,8,0,170,162,
60,73,107,16,183,103,79,0,0,0,205,0,0,0,28,0,17,0,119,101,98,100,101,109,111,47,112,114,111,100,117,99,
116,95,108,105,115,116,95,49,48,50,46,120,109,108,85,84,13,0,7,32,10,236,87,224,235,234,87,39,182,236,87,179,
41,202,47,47,182,227,229,82,80,80,176,1,50,237,108,242,18,115,83,237,2,242,75,18,75,242,83,139,109,244,193,92,
176,52,24,216,20,20,101,38,167,218,25,234,25,217,232,67,152,54,250,32,109,232,6,56,165,38,230,225,209,109,129,95,
119,64,106,34,30,205,134,232,154,193,52,200,19,0,80,75,3,4,20,0,0,0,8,0,170,162,60,73,53,38,55,212,
83,0,0,0,208,0,0,0,28,0,17,0,119,101,98,100,101,109,111,47,112,114,111,100,117,99,116,95,108,105,115,116,
95,49,48,51,46,120,109,108,85,84,13,0,7,32,10,236,87,224,235,234,87,41,182,236,87,179,41,202,47,47,182,227,
229,82,80,80,176,1,50,237,108,242,18,115,83,237,194,19,75,82,139,108,244,193,108,176,28,24,216,20,20,101,38,167,
218,25,232,153,218,232,67,152,54,250,32,61,232,186,157,243,115,18,113,107,182,196,175,217,177,160,32,39,85,193,171,20,
168,2,183,25,230,232,102,128,105,144,63,0,80,75,1,2,23,11,10,0,0,0,0,0,170,162,60,73,0,0,0,0,
0,0,0,0,0,0,0,0,8,0,9,0,0,0,0,0,0,0,16,0,255,65,0,0,0,0,119,101,98,100,101,109,
111,47,85,84,5,0,7,32,10,236,87,80,75,1,2,23,11,20,0,0,0,8,0,170,162,60,73,168,198,156,182,122,
0,0,0,144,0,0,0,18,0,9,0,0,0,0,0,1,0,32,0,182,129,55,0,0,0,119,101,98,100,101,109,111,
47,105,110,100,101,120,46,104,116,109,108,85,84,5,0,7,32,10,236,87,80,75,1,2,23,11,20,0,0,0,8,0,
170,162,60,73,158,225,250,98,89,0,0,0,222,0,0,0,20,0,9,0,0,0,0,0,1,0,32,0,182,129,242,0,
0,0,119,101,98,100,101,109,111,47,99,111,110,116,101,110,116,115,46,120,109,108,85,84,5,0,7,32,10,236,87,80,
75,1,2,23,11,20,0,0,0,8,0,170,162,60,73,169,45,180,0,91,0,0,0,13,1,0,0,28,0,9,0,0,
0,0,0,1,0,32,0,182,129,142,1,0,0,119,101,98,100,101,109,111,47,112,114,111,100,117,99,116,95,108,105,115,
116,95,49,48,49,46,120,109,108,85,84,5,0,7,32,10,236,87,80,75,1,2,23,11,20,0,0,0,8,0,170,162,
60,73,107,16,183,103,79,0,0,0,205,0,0,0,28,0,9,0,0,0,0,0,1,0,32,0,182,129,52,2,0,0,
119,101,98,100,101,109,111,47,112,114,111,100,117,99,116,95,108,105,115,116,95,49,48,50,46,120,109,108,85,84,5,0,
7,32,10,236,87,80,75,1,2,23,11,20,0,0,0,8,0,170,162,60,73,53,38,55,212,83,0,0,0,208,0,0,
0,28,0,9,0,0,0,0,0,1,0,32,0,182,129,206,2,0,0,119,101,98,100,101,109,111,47,112,114,111,100,117,
99,116,95,108,105,115,116,95,49,48,51,46,120,109,108,85,84,5,0,7,32,10,236,87,80,75,5,6,0,0,0,0,
6,0,6,0,204,1,0,0,108,3,0,0,0,0,
};

#endif // USE_SFK_BASE





typedef struct {
    char chunkID[4];        // "RIFF"
    uint32_t chunkSize;     // Size of the entire file minus 8 bytes
    char format[4];         // "WAVE"
    char subchunk1ID[4];    // "fmt "
    uint32_t subchunk1Size; // Size of the format chunk (16 for PCM)
    uint16_t audioFormat;   // Audio format (1 for PCM)
    uint16_t numChannels;   // Number of channels (1 for mono, 2 for stereo)
    uint32_t sampleRate;    // Sample rate (44100 for 44.1 kHz)
    uint32_t byteRate;      // Number of bytes per second
    uint16_t blockAlign;    // Number of bytes per sample frame
    uint16_t bitsPerSample; // Bits per sample (16 for 16-bit audio)
    char subchunk2ID[4];    // "data"
    uint32_t subchunk2Size; // Size of the audio data
} MinWAVHeader;

typedef struct {
    uint16_t audioFormat;   // Audio format (1 for PCM)
    uint16_t numChannels;   // Number of channels (1 for mono, 2 for stereo)
    uint32_t sampleRate;    // Sample rate (44100 for 44.1 kHz)
    uint32_t byteRate;      // Number of bytes per second
    uint16_t blockAlign;    // Number of bytes per sample frame
    uint16_t bitsPerSample; // Bits per sample (16 for 16-bit audio)
} WAVFmtHeader;

class SFKSound
{
public:
   SFKSound ( );

   int setFrom(uchar *pdata, int nsize, char *pname);
       // caller owns memory

   int setOffsetAndCut(int iOffFrames, int iCutFrames);

   char *getid();
   uint  getsize();

uchar *rawptr;
uint   rawlen;

WAVFmtHeader fmt;
uchar *dataptr;
uint   datalen;

uchar *pcur;
uchar *pmax;
char   chunkid[20];
uint   chunksize;
uint   riffChunkSize;
uint   fmtChunkSize;
uint   dataChunkSize;
uint  *dataChunkSizePtr;
};

SFKSound::SFKSound( ) { memset(this, 0, sizeof(*this)); }

int SFKSound::setOffsetAndCut(int iOffFrames, int iCutFrames)
{
   short *pdata = (short*)dataptr;
   int    nfram = datalen/4;

   pdata += iOffFrames;
   nfram -= iOffFrames;
   nfram -= iCutFrames;
   if (nfram < 1)
      return 9;

   dataptr = (uchar*)pdata;
   datalen = nfram * 4;

   return 0;
}

char *SFKSound::getid() {
   if (pcur+4 >= pmax) return str("?");
   chunkid[0]=(char)*pcur++;
   chunkid[1]=(char)*pcur++;
   chunkid[2]=(char)*pcur++;
   chunkid[3]=(char)*pcur++;
   chunkid[4]='\0';
   // printf("getid offs 0x%x = '%s'\n",(pcur-rawptr),chunkid);
   return chunkid;
}

uint SFKSound::getsize() {
   uint n = ((uint)*pcur++) << 0;
   n |= ((uint)*pcur++) << 8;
   n |= ((uint)*pcur++) << 16;
   n |= ((uint)*pcur++) << 24;
   return (int)n;
}

int SFKSound::setFrom(uchar *pdata, int nsize, char *pname)
{
   rawptr=pdata;
   rawlen=nsize;   

   pcur=rawptr;
   pmax=pcur+rawlen;

   if (cs.verbose) {
      printf("%s\n   total file size: %d\n", pname, nsize);
   }

   if (strcmp(getid(), "RIFF")) {
      printf("no valid wav file: %s\n",pname);
      return 10;
   }
   riffChunkSize=getsize();
   if (cs.verbose)
      printf("   riff chunk size: %u (-%d)\n", riffChunkSize, (int)(nsize-riffChunkSize));
   if (strcmp(getid(), "WAVE")) {
      printf("no valid wav file: %s\n",pname);
      return 11;
   }
   while (pcur<pmax) 
   {
      getid();
      if (!strcmp(chunkid, "fmt ")) {
         fmtChunkSize=getsize();
         if (cs.verbose)
            printf("   fmt chunk size : %u\n", fmtChunkSize);
         if (fmtChunkSize<sizeof(WAVFmtHeader)) {
            printf("invalid format chunk size %d: %s\n",fmtChunkSize,pname);
            return 12;
         }
         // we only understand that size
         memcpy(&fmt,pcur,sizeof(WAVFmtHeader));
         // but skip given size
         pcur += fmtChunkSize;
         continue;
      }
      if (strcmp(chunkid, "data")) {
         uint iskipsize=getsize();
         if (cs.verbose)
            printf("   skipping '%s' with %u bytes\n", chunkid, iskipsize);
         pcur+=iskipsize;
         continue;
      }
      // data chunk
      dataChunkSizePtr=(uint*)pcur;
      dataChunkSize=getsize();
      dataptr=pcur;
      datalen=dataChunkSize;
      if (cs.verbose) {
         uint ndataoff = (uint)(pcur-rawptr);
         uint nremain  = rawlen-ndataoff;
         printf("   pcm data size  : %u (offset=%u)\n", datalen, (uint)(pcur-rawptr));
         printf("   bytes from pcm : %u (+%d)\n", nremain, (int)(nremain-datalen));
      }
      break;
   }
   if (dataptr==0) {
      printf("no pcm data found: %s\n",pname);
      return 13;
   }

   return 0;
}









int execTextJoinLines(char *pIn) {
   // join a text file with lines broken by mailing

   // 1. pre-scan for line length maximum
   char *psz = pIn;
   int nLineChars = 0;
   int nLineCharMax = 0;
   while (*psz)
   {
      char c = *psz++;
      if (c == '\r')
         continue;
      if (c == '\n') {
         if (nLineChars > nLineCharMax)
            nLineCharMax = nLineChars;
         nLineChars = 0;
         continue;
      }
      nLineChars++;
   }

   if (!cs.quiet)
      printf("[line break near %d]\n", nLineCharMax);

   // 2. join lines which are broken, pass-through others
   psz = pIn;
   nLineChars = 0;
   while (*psz)
   {
      char c = *psz++;

      if (c == '\r') // drop CR. LF-mapping is done by runtime.
         continue;

      if (c == '\n') {
         // line collected. what to do?
         if (   (nLineChars < nLineCharMax-1)
             || (nLineChars > nLineCharMax)
            )
         {
            fputc(c, fGlblOut);  // not near threshold: do not join
         }
         nLineChars = 0;
         continue;
      } else {
         fputc(c, fGlblOut);
      }

      nLineChars++;
   }
 
   return 0;
}



// rc 5 : lines were truncated
int diffMemText(char *pleft, char *prite,
   char *padd, char *prem, char *psame, int astat[3]
   )
{__
   KeyMap omapFile,omapChain,omapMix;
   omapFile.setcase(cs.usecase);
   omapChain.setcase(cs.usecase);
   omapMix.setcase(cs.usecase);

   int irc=0,isame=0,iadd=0,irem=0;

   // read ref data, reducing dub lines
   char *psz=pleft;
   while (*psz!=0)
   {
      char *peol=psz;
      while (*peol!=0 && *peol!='\n') peol++;
      int ilen=peol-psz;
      if (ilen>MAX_LINE_LEN) {
         ilen = MAX_LINE_LEN;
         irc = 5;
      }
      memcpy(szLineBuf,psz,ilen);
      szLineBuf[ilen]='\0';
      removeCRLF(szLineBuf);
      omapFile.put(szLineBuf, 0);
      omapMix.put(szLineBuf, 0);
      psz=peol;
      if (*psz) psz++;
   }

   // read chain text, reducing dub lines
   psz=prite;
   while (*psz!=0)
   {
      char *peol=psz;
      while (*peol!=0 && *peol!='\n') peol++;
      int ilen=peol-psz;
      if (ilen>MAX_LINE_LEN) {
         ilen = MAX_LINE_LEN;
         irc = 5;
      }
      memcpy(szLineBuf,psz,ilen);
      szLineBuf[ilen]='\0';
      removeCRLF(szLineBuf);
      omapChain.put(szLineBuf, 0);
      omapMix.put(szLineBuf, 0);
      psz=peol;
      if (*psz) psz++;
   }

   // list differences
   for (int i=0; i<omapMix.size(); i++)
   {
      char *pszMixLine = 0;
      omapMix.iget(i, &pszMixLine);
      if (!pszMixLine) continue; // safety
 
      bool bChainSet = omapChain.isset(pszMixLine);
      bool bFileSet  = omapFile.isset(pszMixLine);

      char *ppre = 0;
      char  ccol = ' ';

      if (bChainSet && bFileSet) {
         if (psame)
            ppre = psame;
         ccol = 'b';
         isame++;
      }
      else
      if (bChainSet) {
         iadd++;
         ppre = padd;
         if (!irc) irc=1;
         if (psame)
            ccol = 'g';
      } else {
         irem++;
         ppre = prem;
         if (!irc) irc=1;
         ccol = 'R';
      }

      if (ppre) {
         // chain.print("%s %s\n", ppre, pszMixLine);
         snprintf(szLineBuf, MAX_LINE_LEN, "%s %s", ppre, pszMixLine);
         memset(szAttrBuf, ccol, strlen(szLineBuf));
         if (chain.colany())
            chain.addLine(szLineBuf, szAttrBuf);
         else
            printColorText(szLineBuf, szAttrBuf);
      }
   }

   astat[0] = isame;
   astat[1] = iadd;
   astat[2] = irem;

   return irc;
}

void dospell(char *pszWord, bool bNato, bool bPrefix)
{
   static const char *apszMixed[26] =
   {
      "Alpha", "Bravo",  "Charlie", "Delta",  "Echo",  "Foxtrot",  "Golf", "Hotel",
      "India", "Johnny", "King",    "London", "Mike",  "November", "Oscar", "Peter",
      "Queen", "Roger",  "Sierra",  "Tango",  "Union", "Victor",   "William",
      "X-ray", "Yankee", "Zebra"
   };

   static const char *apszNato[26] =
   {
      "Alpha", "Bravo", "Charlie", "Delta", "Echo", "Foxtrot", "Golf", "Hotel",
      "India", "Juliet", "Kilo", "Lima", "Mike", "November", "Oscar", "Papa",
      "Quebec", "Romeo", "Sierra", "Tango", "Uniform", "Victor", "Whisky",
      "X-ray", "Yankee", "Zulu"
   };

   const char **apsz = bNato ? apszNato : apszMixed;

   char szBuf[20];

   if (pszWord)
   {
      if (bPrefix) {
         if (chain.coldata) {
            chain.addToCurLine(pszWord, str(""));
            chain.addToCurLine(str(" : "), str(""));
         } else {
            printf("%s : ", pszWord);
         }
      }
      while (*pszWord)
      {
         char c = sfktolower(*pszWord++);
 
         if (c >= 'a' && c <= 'z') {
            if (chain.coldata)
               chain.addToCurLine((char*)apsz[c - 'a'], str(""));
            else
               printf("%s ", apsz[c - 'a']);
         } else {
            szBuf[0] = c;
            szBuf[1] = '\0';
            if (chain.coldata)
               chain.addToCurLine(szBuf, str(""));
            else
               printf("%s ", szBuf);
         }
         if (chain.coldata)
            chain.addToCurLine(str(" "), str(""));
      }
   }
   else
   for (int i=0; i<26; i++)
   {
      printf("%-10.10s ", apsz[i]);
      if ((i & 7) == 7)
         printf("\n");
   }
}

#define regetcol acol[((ilevel/2)+bval)&1]

int reformatjson(char *pinbuf, char *poutbuf, char *poutatt, int ioutmax)
{
   int ilevel=0;
   int istate=0;
   int bkeepquotes=1;

   cchar *acol = "hx";
   int icol = strlen(acol);
   int bval = 0;

   char *pout = poutbuf;
   char *pmax = poutbuf+(ioutmax-10);
   char *patt = poutatt;
   char cattr = ' ';

   char *pcur=pinbuf;
   while (*pcur != 0 && pout+10 < pmax)
   {
      cattr = regetcol;

      // handle quotes
      if (istate==0 && *pcur=='\"') {
         // ilevel += 2; cattr = regetcol;
         if (bkeepquotes)
            { *pout++ = *pcur++; *patt++ = cattr; }
         else
            pcur++;
         istate=1;
         continue;
      }
      if (istate==1) {
         if (!strncmp(pcur, "\\\"", 2)) {
            if (bkeepquotes)
               { *pout++ = *pcur++; *patt++ = cattr; }
            else
               pcur++;
            *pout++ = *pcur++; *patt++ = cattr;
            continue;
         }
         if (*pcur!='\"') {
            *pout++ = *pcur++; *patt++ = cattr;
            continue;
         }
         if (bkeepquotes)
            { *pout++ = *pcur++; *patt++ = cattr; }
         else
            pcur++;
         // ilevel -= 2; cattr = regetcol;
         istate=0;
         continue;
      }

      // optimize empty arrays
      if (!strncmp(pcur, "[[]]", 4)) {
         *pout++ = *pcur++; *patt++ = cattr;
         *pout++ = *pcur++; *patt++ = cattr;
         *pout++ = *pcur++; *patt++ = cattr;
         *pout++ = *pcur++; *patt++ = cattr;
         continue;
      }
      if (!strncmp(pcur, "[]", 2)) {
         *pout++ = *pcur++; *patt++ = cattr;
         *pout++ = *pcur++; *patt++ = cattr;
         continue;
      }

      char c = *pcur++;

      switch (c)
      {
         case '[':
         case '{':
            bval=0;
            if (pout > poutbuf
                && (pout[-1] == ' ' || pout[-1] == '\n')) {
            }
            else if (pout == poutbuf) {
            }
            else {
               *pout++ = '\n'; *patt++ = cattr;
               for (int i=0; i<ilevel; i++)
                  { *pout++ = ' '; *patt++ = cattr; }
            }
            *pout++ = c; *patt++ = cattr;
            ilevel += 2; cattr = regetcol;
            *pout++ = '\n'; *patt++ = cattr;
            for (int i=0; i<ilevel; i++)
               { *pout++ = ' '; *patt++ = cattr; }
            break;

         case ']':
         case '}':
            bval=0;
            if (pout > poutbuf && pout[-1] == ' ') {
               ilevel -= 2; cattr = regetcol;
               pout -= 2; patt -= 2;
            } else {
               *pout++ = '\n'; *patt++ = cattr;
               ilevel -= 2; cattr = regetcol;
               for (int i=0; i<ilevel; i++)
                  { *pout++ = ' '; *patt++ = cattr; }
            }
            *pout++ = c; *patt++ = cattr;
            if (*pcur == ',')
               { *pout++ = *pcur++; *patt++ = cattr; }
            *pout++ = '\n'; *patt++ = cattr;
            for (int i=0; i<ilevel; i++)
               { *pout++ = ' '; *patt++ = cattr; }
            break;

         case ',':
            *pout++ = c; *patt++ = cattr;
            *pout++ = '\n'; *patt++ = cattr;
            for (int i=0; i<ilevel; i++)
               { *pout++ = ' '; *patt++ = cattr; }
            bval=0;
            break;

         case ':':
            *pout++ = c; *patt++ = cattr;
            *pout++ = ' '; *patt++ = cattr;
            bval=1;
            break;

         default:
            *pout++ = c; *patt++ = cattr;
            break;
      }
   }
   *pout = '\0'; *patt = '\0';

   if (pout+10 >= pmax)
      return 10;

   return 0;
}





#if (defined(SFKINT) || defined(SFKPIC))
  #include "sfkint.cpp"
#endif // SFKINT





// - - - extmain - - -





// internal check: structure alignments must be same as in sfk.cpp
void getAlignSizes2(int &n1, int &n2, int &n3)
{
   n1 = (int)sizeof(AlignTest1);
   n2 = (int)sizeof(AlignTest2);
   n3 = (int)sizeof(AlignTest3);
}



