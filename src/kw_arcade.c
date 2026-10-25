/* Keen Worlds native arcade bridge. GPL v2 or later.
 * The companion runs original Keen 1-3 in native SDL code. Its DOS EXEs
 * supply tables only. No shell, emulator, shared inventory or world flags.
 */
#include "kw_arcade.h"
#include "id_fs.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_vl.h"
#include "id_rf.h"
#include "kw_music.h"
#include "kw_skip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#if defined(WITH_SDL) && WITH_SDL != 3
#include <SDL.h>
#endif
#if defined(SDL_MAJOR_VERSION) && SDL_MAJOR_VERSION == 2
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
extern char **environ;
#endif

extern const char *fs_userPath;
static bool KW_ArcadePaths(int episode, char *binary, size_t bn, char *data, size_t dn)
{
 if (episode < 1 || episode > 3) return false;
 char *base = SDL_GetBasePath();
 if (!base) return false;
 int n = snprintf(binary, bn, "%skeen-vorticons%s", base,
#ifdef _WIN32
  ".exe"
#else
  ""
#endif
 );
 SDL_free(base);
 int m = snprintf(data, dn, "%s/arcade/keen%d", fs_keenPath, episode);
 return n > 0 && (size_t)n < bn && m > 0 && (size_t)m < dn;
}

KW_ArcadeStatus KW_ArcadeCheck(int episode)
{
 char binary[1024], data[1024], file[32];
 if (!KW_ArcadePaths(episode, binary, sizeof(binary), data, sizeof(data))) return KW_ARCADE_NO_ENGINE;
 FILE *f = fopen(binary, "rb");
 if (!f) return KW_ARCADE_NO_ENGINE;
 fclose(f);
 // The engine supports original v1.31. Test the actual EXE before passing
 // its table data to the interpreter (the full image or LZEXE-packed image).
 snprintf(file, sizeof(file), "KEEN%d.EXE", episode);
 f = FSL_OpenFileInDirCaseInsensitive(data, file, false);
 if (!f) return KW_ARCADE_NO_DATA;
 fseek(f, 0, SEEK_END); long size = ftell(f); fclose(f);
 static const long compressed[] = {51190, 58335, 61599};
 static const long unpacked[] = {100484, 118672, 127616};
 if (size != compressed[episode-1] && size != unpacked[episode-1]) return KW_ARCADE_WRONG_VERSION;
 // Only Keen 1 has an external sound file; Keen 2/3 embed sounds in the EXE.
 const char *required[] = {"EGAHEAD", "EGALATCH", "EGASPRIT", "LEVEL01", "LEVEL80", "SOUNDS"};
 for (unsigned i = 0; i < sizeof(required) / sizeof(required[0]); ++i)
 {
  if (episode != 1 && !strcmp(required[i], "SOUNDS")) continue;
  snprintf(file, sizeof(file), "%s.CK%d", required[i], episode);
  f = FSL_OpenFileInDirCaseInsensitive(data, file, false);
  if (!f) return KW_ARCADE_NO_DATA;
  fclose(f);
 }
 return KW_ARCADE_READY;
}

static bool KW_ArcadeMkdir(const char *path)
{
#ifdef _WIN32
 int result = _mkdir(path);
#else
 int result = mkdir(path, 0755);
#endif
 if (!result) return true;
 struct stat st;
 return errno == EEXIST && !stat(path, &st) && (st.st_mode & S_IFDIR);
}

#ifdef _WIN32
// Windows passes one command line to CreateProcess. Quote every argument,
// doubling backslashes before quotes and before its final quotation mark.
static bool KW_ArcadeQuote(char **out, char *end, const char *arg)
{
 if (*out + 2 >= end) return false;
 *(*out)++ = '"';
 while (*arg)
 {
  int slashes = 0;
  while (*arg == '\\') { ++slashes; ++arg; }
  int copies = (*arg == '"' || !*arg) ? slashes * 2 : slashes;
  if (*out + copies + 3 >= end) return false;
  while (copies--) *(*out)++ = '\\';
  if (!*arg) break;
  if (*arg == '"') *(*out)++ = '\\';
  *(*out)++ = *arg++;
 }
 *(*out)++ = '"'; *(*out)++ = ' '; **out = 0;
 return true;
}
#endif

static KW_ArcadeStatus KW_RunArcade(const char *binary, const char *const *args);

static SDL_Window *KW_ArcadeWindow(void)
{
 SDL_Window *window=SDL_GetKeyboardFocus();
 return window ? window : SDL_GetMouseFocus();
}

static bool KW_ArcadeFullscreen(void)
{
 SDL_Window *window=KW_ArcadeWindow();
 if (window)
 {
  // A desktop fullscreen shortcut can resize the client without changing
  // Keen Worlds' saved option or SDL's fullscreen flag (some compositors).
  SDL_Rect display;
  int index=SDL_GetWindowDisplayIndex(window),x,y,w,h;
  SDL_GetWindowPosition(window,&x,&y);SDL_GetWindowSize(window,&w,&h);
  if (index>=0 && !SDL_GetDisplayBounds(index,&display) && display.w>0 && display.h>0 && w>0 && h>0)
   return x<=display.x && y<=display.y && x+w>=display.x+display.w && y+h>=display.y+display.h;
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)!=0;
 }
 return vl_isFullScreen;
}

KW_ArcadeStatus KW_PlayArcade(int episode)
{
 KW_ArcadeStatus status = KW_ArcadeCheck(episode);
 if (status != KW_ARCADE_READY) return status;
 char binary[1024], data[1024], saves[1024], root[1024], start[32];
 if (!KW_ArcadePaths(episode, binary, sizeof(binary), data, sizeof(data))) return KW_ARCADE_FAILED;
 int n = snprintf(root, sizeof(root), "%s/arcade", fs_userPath);
 if (n < 0 || (size_t)n >= sizeof(root) || !KW_ArcadeMkdir(root)) return KW_ARCADE_FAILED;
 n = snprintf(saves, sizeof(saves), "%s/keen%d", root, episode);
 if (n < 0 || (size_t)n >= sizeof(saves) || !KW_ArcadeMkdir(saves)) return KW_ARCADE_FAILED;
 snprintf(start, sizeof(start), "-startkeen%d", episode);
 bool fullscreen=KW_ArcadeFullscreen();
 const char *args[] = {binary, data, saves, start,
  fullscreen ? "-fullscreen=true" : "-fullscreen=false", NULL};
 printf("Arcade: Keen %d launching %s\n",episode,fullscreen ? "fullscreen" : "windowed");fflush(stdout);
 return KW_RunArcade(binary, args);
}

static KW_ArcadeStatus KW_RunArcade(const char *binary, const char *const *args)
{
 // Keep the running cabin and game objects in memory. Only the companion
 // owns a new simulation and saves, under the user path's arcade/keenN.
 KW_LinesOver();
 SD_MusicOff();
 SD_StopSound();
 SDL_Window *window = KW_ArcadeWindow();
 bool fullscreen=KW_ArcadeFullscreen();
 // Put the cabin behind the menu away before handing the display over.
 RF_ForceRefresh();
 RF_Refresh();
 VL_FadeToBlack();
 if (window) SDL_HideWindow(window);
 bool ok = false;
#ifdef _WIN32
 char command[8192] = {0}, *out = command;
 bool quoted = true;
 for (int i = 0; args[i]; ++i)
  if (!KW_ArcadeQuote(&out, command + sizeof(command), args[i])) quoted = false;
 STARTUPINFOA startup; PROCESS_INFORMATION process;
 memset(&startup, 0, sizeof(startup)); memset(&process, 0, sizeof(process));
 startup.cb = sizeof(startup);
 startup.dwFlags = STARTF_USESTDHANDLES;
 startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
 startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
 startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
 if (quoted && CreateProcessA(binary, command, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &process))
 {
  while (WaitForSingleObject(process.hProcess, 16) == WAIT_TIMEOUT) SDL_PumpEvents();
  DWORD code = 1; GetExitCodeProcess(process.hProcess, &code); ok = code == 0;
  CloseHandle(process.hThread); CloseHandle(process.hProcess);
 }
#else
 pid_t child;
 if (!posix_spawn(&child, binary, NULL, NULL, (char *const *)args, environ))
 {
  int result = 0; pid_t waited;
  do {
   waited = waitpid(child, &result, WNOHANG);
   if (!waited) { SDL_PumpEvents(); SDL_Delay(16); }
  } while (!waited || (waited < 0 && errno == EINTR));
  ok = waited == child && WIFEXITED(result) && WEXITSTATUS(result) == 0;
 }
#endif
 if (window) {
  SDL_ShowWindow(window);
  if (fullscreen) SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
  SDL_RaiseWindow(window);
 }
 IN_PumpEvents();
 KW_ClearPresentationKeys();
 IN_ClearKeysDown();
 RF_ForceRefresh();
 RF_Refresh();
 VL_FadeFromBlack();
 SD_SetLastTimeCount(SD_GetTimeCount());
 return ok ? KW_ARCADE_READY : KW_ARCADE_FAILED;
}
#else
KW_ArcadeStatus KW_ArcadeCheck(int episode) { (void)episode; return KW_ARCADE_NO_ENGINE; }
KW_ArcadeStatus KW_PlayArcade(int episode) { (void)episode; return KW_ARCADE_NO_ENGINE; }
#endif
