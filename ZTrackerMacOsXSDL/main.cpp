
#include "zt.h"
#include <SDL/sdl_syswm.h>

#ifdef DEBUG

BarGraph *playbuff1_bg,*playbuff2_bg,*keybuff_bg;

#endif

// #define USE_BG_IMAGE  // This is for a stupid background image

CScreenUpdateManager screenmanager;
Skin *CurrentSkin = NULL;

bool bScrollLock;

ZTConf zt_globals;
char* zt_directory;
int postAction ();
int preAction ();

bool bIsFullscreen = false;
int pe_modification = 2;
ZTDrawable * pe_buf = NULL;

int need_popup_refresh = 0;

int select_row_start,select_row_end;
int light_pos = 0, need_update_lights = 0;

player *ztPlayer;

UserInterface *UI = NULL, *InstEditorUI = NULL;
/*
 ResourceStream *Skin = NULL;
 BitmapCache *BM_Cache = NULL;
 */

LPSTR cur_dir = NULL;

int file_changed = 0;
char *file_directory;
int modal=0,sel_pat = 0x100,sel_order = 0x0;;
int LastX=0,LastY=0,MousePressX=0,MousePressY=0,MouseBttnL=0,MouseBttnR=0;

int last_cmd_keyjazz=0,last_keyjazz=0;

int fast_update=0, FU_x=0, FU_y=0, FU_dx=0, FU_dy=0;

int Force_CON_Reactivated=0;
int select_track_start,select_track_end;
int selected=0,faded=0,oldhead=0;

int zclear_flag, zclear_presscount;
int already_changed_default_directory = 0;
char ls_filename[256];

//int default_midistopstart = 1;
//int default_midiclock = 1;


int updated=1;
int cur_state,need_refresh=1,need_update=0;
int cur_volume_percent = 100;
int cur_naturalization_percent = 0;
int do_exit = 0;
int editing = 0;
int cur_edit_order = 0;
int cur_edit_track = 0;
int cur_edit_col   = 0;
int cur_edit_row   = 0;
int cur_edit_row_disp = 0;
int cur_edit_pattern = 0;
int cur_edit_track_disp = 0;
int cur_edit_column = 0;

extern bool bMouseIsDown = false;

int cur_inst = 0;
int fixmouse=0;

char *col_desc[41];

int base_octave = 4;
int cur_step = 1;

int keypress=0;
int keywait = 0;
int keytimer = 0x11;
int keyID = 0;

int status_change = 0;

KeyBuffer Keys;
//conf Config;//,ColorsFile;

int clear_popup = 0;

edit_col edit_cols[41];

//int BeatsPerMinute = 138;
//int TicksPerBeat   = 8; // Rows per beat;

int key_jazz = 0x0;
mbuf jazz[512];

int attempt_fullscreen_toggle();

char *statusmsg = " ";
char szStatmsg[1024];

//unsigned long numMidiDevs;

UserInterface *UI_Toolbar = NULL;
Bitmap *bmToolbarRepeater = NULL;

event blank_event;
instrument blank_instrument(0);

zt_module *song;
char zt_filename[1024];
//pattern patterns[1];

ZTScreen *S;
ZTScreen *RealScreen = NULL;
int doredraw=0,colors=0;

CClipboard *clipboard;
WStack window_stack;

bool bDontKeyRepeat=false;

midiOut *MidiOut = NULL;
midiIn *MidiIn = NULL;

CUI_Page *ActivePage = NULL, *LastPage = NULL, *PopupWindow = NULL;

CUI_About *UIP_About = NULL;
CUI_InstEditor *UIP_InstEditor = NULL;
CUI_Logoscreen *UIP_Logoscreen = NULL;
CUI_Loadscreen *UIP_Loadscreen = NULL;
CUI_Savescreen  *UIP_Savescreen = NULL;
CUI_Ordereditor *UIP_Ordereditor = NULL;
CUI_Playsong *UIP_Playsong = NULL;
CUI_Songconfig *UIP_Songconfig = NULL;
CUI_Sysconfig *UIP_Sysconfig = NULL;
CUI_Config *UIP_Config = NULL;
CUI_Patterneditor *UIP_Patterneditor = NULL;
CUI_PEParms *UIP_PEParms = NULL;
CUI_PEVol *UIP_PEVol = NULL;
CUI_PENature *UIP_PENature = NULL;
CUI_SliderInput *UIP_SliderInput = NULL;
CUI_LoadMsg *UIP_LoadMsg = NULL;
CUI_SaveMsg *UIP_SaveMsg = NULL;
CUI_NewSong *UIP_NewSong = NULL;
CUI_RUSure *UIP_RUSure = NULL;
CUI_Help *UIP_Help = NULL;
CUI_SongDuration *UIP_SongDuration = NULL;
CUI_SongMessage *UIP_SongMessage = NULL;
CUI_Arpeggioeditor *UIP_Arpeggioeditor = NULL;
CUI_Midimacroeditor *UIP_Midimacroeditor = NULL;

int check_ext(const char *str, const char *ext) {
    int i,j,k=0;
    i=strlen(str); j=strlen(ext);
    i-=j;
    if (i<0) return 0;
    while(str[i] && ext[k]) {
        if (tolower(str[i]) != tolower(ext[k]))
            return 0;
        i++; k++;
    }
    if (k==j)
        return 1;
    else
        return 0;
}

void reset_editor(void) {
    cur_edit_col = cur_edit_column = cur_edit_row = cur_edit_pattern = 0;
    cur_edit_row_disp = cur_edit_pattern = cur_edit_track = cur_edit_track_disp = 0;
}

int lock_mutex(HANDLE hMutex, int timeout) {
    int result;
#warning TODO
//    result = WaitForSingleObject(hMutex, timeout);
//    if (result == WAIT_OBJECT_0)
//        return 1;
    return 0;
}

int unlock_mutex(HANDLE hMutex) {
#warning TODO
    return 0;
//    return ReleaseMutex(hMutex);
}


void popup_window(CUI_Page *page) {
    if (page->UI)
        page->UI->full_refresh();
    window_stack.push(page);
    screenmanager.UpdateAll();
    modal++;
}
void close_popup_window(void) {
    if (!window_stack.isempty()) {
        window_stack.pop();
        clear_popup = 1;
        modal--;
    }
    if (ActivePage->UI)
        ActivePage->UI->full_refresh();
    //    screenmanager.UpdateAll();
    doredraw++;
    need_refresh++;
}
void switch_page(CUI_Page *page) {
    LastPage = ActivePage;
    if (LastPage)
        LastPage->leave();
    ActivePage = page;
    ActivePage->enter();
    if (ActivePage->UI)
        ActivePage->UI->full_refresh();
    screenmanager.UpdateAll();
    need_refresh++;
}

int zcmp(char *s1, char *s2) {
    int i=0;
    if (!s1 || !s2) return 0;
    while (s1[i]!=0 && s2[i]!=0)
        if (s1[i] != s2[i])
            return 0;
        else
            i++;
    return -1;
}

int zcmpi(char *s1, char *s2) {
    int i=0;
    if (!s1 || !s2) return 0;
    while (s1[i]!=0 && s2[i]!=0)
        if (tolower(s1[i]) != tolower(s2[i]))
            return 0;
        else
            i++;
    return -1;
}


int checkclick(int x1, int y1, int x2, int y2) {
    if (MousePressX>=x1 && MousePressX<=x2 &&
        MousePressY>=y1 && MousePressY<=y2
        )
        return 1;
    return 0;
}
int checkmousepos(int x1, int y1, int x2, int y2) {
    if (LastX>=x1 && LastX<=x2 &&
        LastY>=y1 && LastY<=y2
        )
        return 1;
    return 0;
}
Bitmap *load_cached_bitmap(char *name) {
    return NULL;
}
Bitmap *load_bitmap(char *name) {
    return NULL;
}

WStackNode::WStackNode(CUI_Page *p) {
    page = p;
    p->enter();
    next = NULL;
}

WStackNode::~WStackNode() {
    page->leave();
}




/*
 int loadconf(void) {
 char * temp;
 
 
 return 1;
 }
 */

char *hex2note(char *str,unsigned char note) {   /* Thanks to FSM for great ideas!! */
    char szLetters[] = "C-C#D-D#E-F-F#G-G#A-A#B-";
    if (note<0x80) {
        str[0]=szLetters[2*(note%12)];
        str[1]=szLetters[2*(note%12)+1];
        str[2]='0'+(note/12);
    } else {
        switch(note) {
            case 0x81:
                str[0]=str[1]=str[2]='^'; /* Note cut */
                break;
            case 0x82:
                str[0]=str[1]=str[2]='='; /* Note off */
                break;
            default:                      /* Unknown or blank */
                str[0]=str[1]=str[2]='.';
                break;
        }
    }
    return str;
}
void fadeOut(float step, Screen *S) {
    /*
     for (float f=1;f>0;f-=step) {
     S->setGammaFade(f,f,f);
     SDL_Delay(1);
     }
     */
}
void fadeIn(float step, Screen *S) {
    /*
     for (float f=0;f<=1;f+=step) {
     S->setGammaFade(f,f,f);
     SDL_Delay(1);
     }
     */
}
void setGamma(float f, Screen *S) {
    //    S->setGammaFade(f,f,f);
}

void status(char *msg,ZTDrawable *S) {
    printBG(col(3),row(9),"                                                                            ",COLORS.Text,COLORS.Background,S);
    printBGCC(col(3),row(9),msg,COLORS.Text,COLORS.Background,S);
    screenmanager.Update(col(3),row(9),col(80)-1,row(10));
}
void status(ZTDrawable *S) {
    status(statusmsg,S);
}
void update_status(ZTDrawable *S) {
    int i,o=0;
    char str[10];
    if (ztPlayer->playing) {
        if (ztPlayer->playmode) {
            char time[64],time2[64];
            int sec;
            sec = calcSongSeconds(ztPlayer->playing_cur_row, ztPlayer->playing_cur_order);
            sprintf(time2, "|H|%.2d|U|:|H|%.2d|U|",sec/60,sec%60);
            sec = calcSongSeconds();
            sprintf(time, "%s/|H|%.2d|U|:|H|%.2d|U|",time2,sec/60,sec%60);
            sprintf(szStatmsg,"Playing, Ord: |H|%.3d|U|/|H|%.3d|U|, Pat: |H|%.3d|U|/|H|255|U|, Row: |H|%.3d|U|/|H|%.3d|U|, Time: %s  ",ztPlayer->playing_cur_order,ztPlayer->num_real_orders,ztPlayer->playing_cur_pattern,ztPlayer->playing_cur_row,song->patterns[ztPlayer->playing_cur_pattern]->length,time);
        } else
            sprintf(szStatmsg,"Looping, Pattern: |H|%.3d|U|/|H|255|U|, Row: |H|%.3d|U|/|H|%.3d|U|  ",ztPlayer->playing_cur_pattern,ztPlayer->playing_cur_row,song->patterns[ztPlayer->playing_cur_pattern]->length);
        statusmsg = szStatmsg;
    }
    if (S->lock()==0) {
        status(S);
        if (cur_state == STATE_PEDIT) {
            if (cur_edit_pattern == ztPlayer->playing_cur_pattern) {
                for(i=cur_edit_row_disp;i<(cur_edit_row_disp+PATTERN_EDIT_ROWS);i++) {
                    sprintf(str,"%.3d",i);
                    if (i == ztPlayer->playing_cur_row)
                        printBG(col(1),row(15+o),str,COLORS.Highlight,COLORS.Background,S);
                    else
                        printBG(col(1),row(15+o),str,COLORS.Text,COLORS.Background,S);
                    o++;
                }
            }
        }
        if (ztPlayer->playing_cur_row == 1 ) {
            if(cur_state == STATE_ORDER)
                need_refresh++;
            draw_status_vars(S);
        }
        status_change = 0;
        //updated++;
        screenmanager.UpdateWH(col(1), row(15),3*8,PATTERN_EDIT_ROWS*8);
        S->unlock();
    }
}

void draw_status_vars(ZTDrawable *S) {
    char s[64];
    sprintf(s,"%.2d",song->instruments[cur_inst]->channel+1);
    printBG(col(50),row(5),s,COLORS.Data,COLORS.Black,S); // Channel
    sprintf(s,"%.1d",base_octave);
    printBG(col(50),row(6),s,COLORS.Data,COLORS.Black,S); // Octave
    sprintf(s,"%.3d/255",cur_edit_order);
    printBG(col(12),row(5),s,COLORS.Data,COLORS.Black,S); // ORDER
    sprintf(s,"%.3d/255",cur_edit_pattern);
    printBG(col(12),row(6),s,COLORS.Data,COLORS.Black,S); // PATTERN
    sprintf(s,"%.3d/%.3d",cur_edit_row,song->patterns[cur_edit_pattern]->length-1);
    printBG(col(12),row(7),s,COLORS.Data,COLORS.Black,S); // ROW
    sprintf(s,"%.3d/%.3d",song->tpb,song->bpm);
    if (ztPlayer)
        if (ztPlayer->playing)
            sprintf(s,"%.3d/%.3d",ztPlayer->tpb,ztPlayer->bpm);
    printBG(col(50),row(4),s,COLORS.Data,COLORS.Black,S); // ORDER
    sprintf(s,"%.2d:                       ",cur_inst);
    printBG(col(50),row(3),s,COLORS.Data,COLORS.Black,S); // ORDER
    
    sprintf(s,"%.2d: %.24s",cur_inst,song->instruments[cur_inst]->title);
    printBG(col(50),row(3),s,COLORS.Data,COLORS.Black,S); // ORDER
    
    screenmanager.Update( col(10), row(2), col(79), row(10) );
    
}

void draw_status(ZTDrawable *S) { /* S MUST BE LOCKED! */
    char fn[256];;
#ifdef USE_BG_IMAGE
    S->unlock();
    S->copy(BG_IMAGE,0,0);
    S->lock();
#endif
    print(col(2),row(3),"Song Name",COLORS.Text,S);
    print(col(2),row(4),"File Name",COLORS.Text,S);
    print(col(2),row(5),"    Order",COLORS.Text,S);
    print(col(2),row(6),"  Pattern",COLORS.Text,S);
    print(col(2),row(7),"      Row",COLORS.Text,S);
    
    print(col(38),row(3)," Instrument",COLORS.Text,S);
    print(col(38),row(4),"    TPB/BPM",COLORS.Text,S);
    print(col(38),row(5),"    Channel",COLORS.Text,S);
    print(col(38),row(6),"     Octave",COLORS.Text,S);
    
    printline(col(12),row(2),0x86,25,COLORS.Lowlight,S);
    printline(col(19),row(5),0x81,18,COLORS.Highlight,S);
    printBG(col(12),row(3),"                         ",COLORS.Data,COLORS.Black,S);
    printBG(col(12),row(3),(char *)song->title,COLORS.Data,COLORS.Black,S);
    printBG(col(12),row(4),"                         ",COLORS.Data,COLORS.Black,S);
    strcpy(fn,(const char *)song->filename);
    fn[25] = 0;
    printBG(col(12),row(4),(char *)fn,COLORS.Data,COLORS.Black,S);
    
    draw_status_vars(S);
    
    //  sprintf(s,"%.2d: %.24s",cur_inst,song->instruments[cur_inst]->title);
    //  printBG(col(50),row(3),s,COLORS.Data,COLORS.Black,S); // ORDER
    
    printline(col(12),row(8),0x81,7,COLORS.Highlight,S);
    
    for (int i=3;i<=7;i++)
        printchar(col(11),row(i),0x84,COLORS.Lowlight,S);
    for (int i=5;i<=7;i++)
        printchar(col(19),row(i),0x83,COLORS.Highlight,S);
    for (int i=3;i<=4;i++)
        printchar(col(37),row(i),0x83,COLORS.Highlight,S);
    
    printline(col(50),row(2),0x86,28,COLORS.Lowlight,S);         // Over inst
    
    printchar(col(78),row(3),0x83,COLORS.Highlight,S); // end of inst
    
    printchar(col(57),row(4),0x83,COLORS.Highlight,S);
    printchar(col(52),row(5),0x83,COLORS.Highlight,S);
    printchar(col(51),row(6),0x83,COLORS.Highlight,S);
    printchar(col(51),row(6),0x81,COLORS.Highlight,S);
    
    printchar(col(50),row(7),0x81,COLORS.Highlight,S);
    
    printline(col(57),row(4),0x81,21,COLORS.Highlight,S);       // Under inst
    
    printline(col(52),row(5),0x81,5,COLORS.Highlight,S);
    
    for (int i=3;i<7;i++)
        printchar(col(49),row(i),0x84,COLORS.Lowlight,S);
    
    screenmanager.Update( col(10), row(2), col(79), row(10) );
}


void doquit() {
    do_exit = 1;
}

void quit() {
    UIP_RUSure->str = " Sure to quit?";
    UIP_RUSure->OnYes = (VFunc)doquit;
    popup_window(UIP_RUSure);
}

void global_keys(ZTDrawable *S)
{    
    unsigned int key,clear=0,kstate;
    
    int command = CMD_NONE,full=0;
    
    key = Keys.checkkey();
    
    kstate = Keys.getstate();
    
    if (!key)
    {
        return;
    }
    
    if (!modal)
    {
        switch(key) {
            case DIK_RIGHT:
                if (key==DIK_RIGHT && kstate == KS_CTRL)
                    if (ztPlayer->playing)
                        ztPlayer->ffwd();
                break;
            case DIK_LEFT:
                if (key==DIK_LEFT && kstate == KS_CTRL)
                    if (ztPlayer->playing)
                        ztPlayer->rewind();
                break;
                
            case DIK_RETURN:
                if (kstate & KS_ALT) {
                    attempt_fullscreen_toggle();
                    key = Keys.getkey();
                    clear++;
                    doredraw++;
                    if (ActivePage->UI)
                        ActivePage->UI->full_refresh();
                    screenmanager.UpdateAll();
                    need_refresh++; full++;
                    //SDL_WM_ToggleFullScreen(S->surface);
                }
                break;
            case DIK_SCROLL:
                if (bScrollLock)
                    bScrollLock = 0;
                else
                    bScrollLock = 1;
                break;
            case DIK_Q:
                if (kstate & KS_ALT && kstate & KS_CTRL) {
                    command=CMD_QUIT;
                    key = Keys.getkey();
                }
                break;
            case DIK_N: // new song
                if (kstate & KS_ALT) {
                    popup_window(UIP_NewSong);
                    key = Keys.getkey();
                    clear++;
                }
                break;
            case DIK_P: // song duration
                if (kstate & KS_ALT) {
                    popup_window(UIP_SongDuration);
                    key = Keys.getkey();
                    clear++;
                }
                break;
            case DIK_F7: // Play from Order
                if (kstate & KS_SHIFT) {
                    command = CMD_PLAY_ORDER;
                }
                break;
            case DIK_F4:
                if (kstate & KS_CTRL) {
                    command=CMD_SWITCH_MIDIMACEDIT;
                }
                break;
            case DIK_F9: // load
                if (kstate & KS_CTRL) {
                    command = CMD_SWITCH_LOAD;
                    key = Keys.getkey();
                } else
                    if (kstate & KS_SHIFT) {
                        command = CMD_HARD_PANIC;
                    }
                break;
            case DIK_F10: // save
                if (kstate & KS_CTRL) {
                    command = CMD_SWITCH_SAVE;
                    key = Keys.getkey();
                }
                else if (kstate == KS_NO_SHIFT_KEYS) {
                    command = CMD_SWITCH_SONGMSG;
                }
                break;
            case DIK_F11: // Set Current Order to Playing order or current pattern
                if(kstate & KS_SHIFT) {
                    if(ztPlayer->playing) {
                        cur_edit_order = ztPlayer->cur_order;
                    }
                    else {
                        int i;
                        for(i = 0; i < 255; i++) {
                            if(song->orderlist[i] == cur_edit_pattern) {
                                i = -1;
                                break;
                            }
                        }
                        if (i == -1)
                            for(int i = cur_edit_order+1;; i++) {
                                if(i == cur_edit_order)
                                    break;
                                if(i == 255) {
                                    i = 0;
                                }
                                if(song->orderlist[i] == cur_edit_pattern) {
                                    cur_edit_order = i;
                                    draw_status_vars(S);
                                    break;
                                }
                            }
                    }
                }
                break;
            case DIK_S: // save
                if (kstate & KS_CTRL) {
                    if (song->filename[0]) {
                        if (song->filename[0] != ' ') {
                            popup_window(UIP_SaveMsg);
                        }
                    } else
                        command = CMD_SWITCH_SAVE;
                    key = Keys.getkey();
                    clear++;
                }
                break;
            case DIK_F12:
                if (kstate & KS_ALT)
                    command = CMD_SWITCH_ABOUT;
                //else if (kstate & KS_CTRL)
                //    command = CMD_SWITCH_CONFIG;
                else
                    command = CMD_SWITCH_SYSCONF;
                break;
                
        }
        if (kstate == KS_NO_SHIFT_KEYS)
            switch(key) {
                case DIK_F1: command=CMD_SWITCH_HELP; break;
                case DIK_F2: command=CMD_SWITCH_PEDIT; break;
                case DIK_F3: command=CMD_SWITCH_IEDIT; break;
                case DIK_F4: command=CMD_SWITCH_ARPEDIT; break;
                    //case DIK_F10: command=CMD_SWITCH_SONGCONF; break;
                case DIK_F11: command=CMD_SWITCH_SONGCONF; break;
                case DIK_F5: command = CMD_PLAY;  break;
                case DIK_F6: command = CMD_PLAY_PAT;  break;
                case DIK_F7: command = CMD_PLAY_PAT_LINE;  break;
                case DIK_F8: command = CMD_STOP;  break;
                case DIK_F9: command = CMD_PANIC;  break;
            }
    }
    switch(command) {
        case CMD_QUIT: quit(); break;
        case CMD_SWITCH_LOAD:
            switch_page(UIP_Loadscreen);
            clear++; doredraw++;
            break;
        case CMD_SWITCH_SAVE:
            switch_page(UIP_Savescreen);
            clear++; doredraw++;
            break;
        case CMD_SWITCH_SONGCONF:
            switch_page(UIP_Songconfig);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_SONGMSG:
            switch_page(UIP_SongMessage);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_ARPEDIT:
            switch_page(UIP_Arpeggioeditor);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_MIDIMACEDIT:
            switch_page(UIP_Midimacroeditor);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_PEDIT:
            if (cur_state == STATE_PEDIT) {
                popup_window(UIP_PEParms); clear++;
                doredraw++;
            } else {
                switch_page(UIP_Patterneditor);
                doredraw++; clear++;
            }
            break;
        case CMD_SWITCH_IEDIT:
            switch_page(UIP_InstEditor);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_ORDERLIST:
            switch_page(UIP_Ordereditor);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_SYSCONF:
            switch_page(UIP_Sysconfig);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_CONFIG:
            switch_page(UIP_Config);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_ABOUT:
            switch_page(UIP_About);
            doredraw++; clear++;
            break;
        case CMD_SWITCH_HELP:
            switch_page(UIP_Help);
            doredraw++; clear++;
            break;
        case CMD_PLAY:
            draw_status_vars(S);
            if (cur_state != STATE_PLAY) {
                switch_page(UIP_Playsong);
                doredraw++;
                clear++;
            }
            if (song->orderlist[0] != 0x100 && !ztPlayer->playing) {
                ztPlayer->play(0,cur_edit_pattern,1);
                clear++;
            }
            break;
        case CMD_PLAY_ORDER:
            draw_status_vars(S);
            if(song->orderlist[cur_edit_order] != 0x100)
            {
                ztPlayer->play(0,cur_edit_order,3);
                clear++;
            }
            break;
        case CMD_PLAY_PAT:
            draw_status_vars(S);
            ztPlayer->play(0,cur_edit_pattern,0);
            clear++;
            break;
        case CMD_PLAY_PAT_LINE:
            if ((cur_state == STATE_SONG_CONFIG)||(cur_state == STATE_ORDER )&& sel_pat < 0x100) {
                ztPlayer->play(0,sel_order,3);
            } else {
                ztPlayer->play(cur_edit_row,cur_edit_pattern,2);
            }
            clear++;
            break;
        case CMD_STOP: ztPlayer->stop(); draw_status_vars(S);clear++; break;
        case CMD_PANIC: MidiOut->panic(); draw_status_vars(S); statusmsg = "Panic"; clear++; break;
        case CMD_HARD_PANIC: MidiOut->hardpanic(); draw_status_vars(S); statusmsg = "Hard Panic"; clear++; break;
        default:
            break;
    }
    
    
    if (clear) {
        Keys.flush();
        midiInQueue.clear();
        if (full)
            redrawscreen(S);
        if (S->lock()==0) {
            status(S);
            //updated=2;
            screenmanager.UpdateAll();
            S->unlock();
        }
    }
}

void redrawscreen(ZTDrawable *S)
{
    char header[80];
    
    sprintf(header,"ztracker v%d.%d by Christopher Micali 2000-2001",VER_MAJ,VER_MIN);
    
    if (S->lock()==0)
    {
        S->fillRect(0,0,CONSOLE_WIDTH-1,CONSOLE_HEIGHT-1/*410*/,COLORS.Background);
        
        //      S->fillRect(0,465,CONSOLE_WIDTH,479,COLORS.LCDLow);
        
        printline(0,0,0x81,CONSOLE_WIDTH/8,COLORS.Highlight,S);
        printchar(0,0,0x80,COLORS.Highlight,S);
        
        for(int y=0;y<CONSOLE_HEIGHT/8;y++)
        {
            printchar(0,row(y),131,COLORS.Highlight,S);
        }
        
        print(col(textcenter(header)),row(1),header,COLORS.Text,S);
        //      sprintf(header,"015/128 - C:012 N:013");
        //      printLCD(col(2),row(55),header,S);
        draw_status(S);
        
        if (ActivePage)
        {
            if (ActivePage->UI)
            {
                ActivePage->UI->full_refresh();
                //                ActivePage->UI->draw(S);
            }
            //            ActivePage->draw(S);
        }
        
        S->unlock();
    }
    
    S->copy(CurrentSkin->bmToolbar, 0,CONSOLE_HEIGHT-55/*425*/,0,0,640,55);
    
    int remblk = (CONSOLE_WIDTH-640)/80;
    
    for(int cx = 0; cx<remblk;cx++)
    {
        S->copy(bmToolbarRepeater, 640+(cx*80), CONSOLE_HEIGHT-55,0,0,80,55);
    }
    
    UI_Toolbar->full_refresh();
    
    doredraw=0;
    //updated=2;
    
    screenmanager.UpdateAll();
}

void make_toolbar(void) {
    
    Bitmap *BUTTONS;
    GfxButton *gb;
    int id=0;
    
    UI_Toolbar->reset();
    
    // This is beautiful isnt it?   I should be punished for this
    
#define BUTTON_X_SIZE 28
#define BUTTON_Y_SIZE 16
#define grab_buttons(gb,x,y) gb->bmDefault = newBitmap(BUTTON_X_SIZE,BUTTON_Y_SIZE,1); \
gb->bmDefault->copy(BUTTONS,0,0,x,y,x+BUTTON_X_SIZE,y+BUTTON_X_SIZE); \
gb->bmOnClick = newBitmap(BUTTON_X_SIZE,BUTTON_Y_SIZE,1); \
gb->bmOnClick->copy(BUTTONS,0,0,x,y+BUTTON_Y_SIZE*2 +2,x+BUTTON_X_SIZE,y+BUTTON_Y_SIZE + (BUTTON_X_SIZE*2 +2)); \
gb->bmOnMouseOver = newBitmap(BUTTON_X_SIZE,BUTTON_Y_SIZE,1); \
gb->bmOnMouseOver->copy(BUTTONS,0,0,x,y + (BUTTON_Y_SIZE*4 +4),x + BUTTON_X_SIZE,y + (BUTTON_Y_SIZE*4 +4) + BUTTON_Y_SIZE); \

#define make_button(gb,x,y,BUTTON_X, BUTTON_Y) gb->bmDefault = newBitmap(BUTTON_X,BUTTON_Y,1); \
gb->bmDefault->copy(BUTTONS,0,0,x,y,x+BUTTON_X,y+BUTTON_X); \
gb->bmOnClick = newBitmap(BUTTON_X,BUTTON_Y,1); \
gb->bmOnClick->copy(BUTTONS,0,0,x,y+BUTTON_Y,x+BUTTON_X,y+BUTTON_Y + (BUTTON_Y ));
    
    /* Play Button */
    
    BUTTONS = CurrentSkin->bmButtons;//load_bitmap("buttons.bmp");
    
    delete bmToolbarRepeater;
    bmToolbarRepeater = newBitmap(80,55);
    bmToolbarRepeater->copy(BUTTONS,0,0, 165,0, 165+80,55);
    
    
    /* Play */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+476;    gb->y = CONSOLE_HEIGHT-(480-442);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,0,0);
    gb->StuffKey = DIK_F5;
    UI_Toolbar->add_element(gb,id++);
    
    /* Load */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+505;    gb->y = CONSOLE_HEIGHT-(480-442);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,29,0);
    gb->StuffKey = DIK_F9;
    gb->StuffKeyState = KMOD_CTRL;
    UI_Toolbar->add_element(gb,id++);
    
    /* Conf */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+533;    gb->y = CONSOLE_HEIGHT-(480-442);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,57,0);
    gb->StuffKey = DIK_F11;
    UI_Toolbar->add_element(gb,id++);
    
    /* Stop */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+476;    gb->y = CONSOLE_HEIGHT-(480-459);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,0,17);
    gb->StuffKey = DIK_F8;
    UI_Toolbar->add_element(gb,id++);
    
    /* Save */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+505;    gb->y = CONSOLE_HEIGHT-(480-459);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,29,17);
    // gb->StuffKey = DIK_S;         // bugfix #3, tlr
    // gb->StuffKeyState = KS_CTRL;  // bugfix #3, tlr
    gb->StuffKey = DIK_F10;          // bugfix #3, tlr
    gb->StuffKeyState = KMOD_CTRL;     // bugfix #3, tlr
    UI_Toolbar->add_element(gb,id++);
    
    /* Quit */
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-640+533;    gb->y = CONSOLE_HEIGHT-(480-459);    gb->xsize = 28; gb->ysize = 16;
    grab_buttons(gb,57,17);
    gb->StuffKey = DIK_Q;
    gb->StuffKeyState = KMOD_CTRL | KMOD_ALT;
    UI_Toolbar->add_element(gb,id++);
    
    
    /* ZT About */
    
    gb = new GfxButton();
    gb->x = CONSOLE_WIDTH-80; gb->y = CONSOLE_HEIGHT-55; gb->xsize = 80; gb->ysize = 55;
    make_button(gb,85,0,80,55)
    // gb->StuffKey = DIK_F1;                 // bugfix #2, tlr
    // gb->StuffKeyState = KS_CTRL | KS_ALT;  // bugfix #2, tlr
    gb->StuffKey = DIK_F12;                   // bugfix #2, tlr
    gb->StuffKeyState = KMOD_ALT;               // bugfix #2, tlr
    UI_Toolbar->add_element(gb,id++);
    
    
    //    RELEASEINT(BUTTONS);
    
#ifdef DEBUG
    playbuff1_bg = new BarGraph();
    playbuff1_bg->x = 2; playbuff1_bg->y = 442; playbuff1_bg->xsize = 80; playbuff1_bg->ysize = 12;
    playbuff1_bg->max = 4200; playbuff1_bg->trackmaxval = 1;
    UI_Toolbar->add_element(playbuff1_bg,id++);
    
    playbuff2_bg = new BarGraph();
    playbuff2_bg->x = 2; playbuff2_bg->y = 456; playbuff2_bg->xsize = 80; playbuff2_bg->ysize = 12;
    playbuff2_bg->max = 4200; playbuff2_bg->trackmaxval = 1;
    UI_Toolbar->add_element(playbuff2_bg,id++);
    
    keybuff_bg = new BarGraph();
    keybuff_bg->x = 37; keybuff_bg->y = 470; keybuff_bg->xsize = 80-35; keybuff_bg->ysize = 9;
    keybuff_bg->max = Keys.maxsize; keybuff_bg->trackmaxval = 1;
    UI_Toolbar->add_element(keybuff_bg,id++);
    
#endif
    /*
     LCDDisplay *l;
     debug_display = l = new LCDDisplay;
     l->x = 16; l->y = 16;
     l->length = 20; l->str = "debug";
     UI_Toolbar->add_element(l, id++);
     */
}

void encode(char *str, char w[256])
{
    char *q, *r;
    //*w = (char*)malloc(strlen(str));
    q = w;
    r = str;
    for(;*r != '\0'; r++, q++) {
        if((*r >= 'a' && *r <= 'z') || (*r >= 'A' && *r <= 'Z') || (*r >= '0' && *r <= '9')) {
            *q = *r;
        }
        else
            *q = '_';
    }
    *q = '\0';
    //return(w);
}

void setup_midi()
{
    char *name, *temp, szKey[256], tt[256];
    
    conf *Config = zt_globals.Config;
    
    unsigned int i,j;
    
    //	tt = NULL;
    for (j=0;j<MidiOut->numOuputDevices;j++) {
        
        name = MidiOut->outputDevices[j]->szName;
        //		if(tt != NULL)
        //			free(tt);
        encode(name, tt);
        
        sprintf(szKey,"latency_%s",tt);
        temp = Config->get(&szKey[0]);
        if(temp) {
            MidiOut->outputDevices[j]->delay_ticks = atoi(temp);
        }
        sprintf(szKey,"alias_%s",tt);
        temp = Config->get(&szKey[0]);
        if(temp && strlen(temp) > 0) {
            MidiOut->outputDevices[j]->alias = strdup(temp);
        }
        else MidiOut->outputDevices[j]->alias = strdup("");
        sprintf(szKey,"bank_%s",tt);
        temp = Config->get(szKey);
        if(temp) {
            if(temp[0] == 'Y' || temp[0] == 'y')
                ((MidiOutputDevice*)(MidiOut->outputDevices[j]))->reverse_bank_select = 1;
            else
                ((MidiOutputDevice*)(MidiOut->outputDevices[j]))->reverse_bank_select = 0;
        }
    }
    //	if(tt != NULL)
    //		free(tt);
    
    if (zt_globals.auto_open_midi) {
        for (i=0;i<MAX_MIDI_OUTS;i++){
            sprintf(szKey,"open_out_device_%d",i);
            name = zt_globals.Config->get(szKey);
            if (name) {
                for (j=0;j<MidiOut->numOuputDevices;j++) {
                    if (zcmp(MidiOut->outputDevices[j]->szName,name)) {
                        MidiOut->AddDevice(j);
                    }
                }
            }
        }
        for (i=0;i<MAX_MIDI_INS;i++){
            sprintf(szKey,"open_in_device_%d",i);
            name = Config->get(&szKey[0]);
            if (name) {
                for (j=0;j<MidiIn->numMidiDevs;j++) {
                    if (zcmp(MidiIn->midiInDev[j]->szName,name)) {
                        MidiIn->AddDevice(j);
                    }
                }
            }
        }
    }
}

int initGFX () {  // Init functions
    //    TOOLBAR = load_cached_bitmap("toolbar.bmp");
#ifdef USE_BG_IMAGE
    //    BG_IMAGE = load_cached_bitmap("bgimage.bmp");
#endif
    // Preload to avoid wierd crashes
    //    Bitmap *img = load_cached_bitmap("save.bmp");
    //    img = load_cached_bitmap("load.bmp");
    //    img = load_cached_bitmap("about.bmp");
    
    //   setDefaultColors(S);
    
    /*
     MOUSEBACK=newBitmap(12,20,0);
     VS = newBitmap(CONSOLE_WIDTH,CONSOLE_HEIGHT);
     VS->clear(0);
     MOUSEBACK->clear(0);
     LastX=C->getMouse()->getX();
     LastY=C->getMouse()->getY();
     */
    
    
    make_toolbar();
    
    switch_page(UIP_Logoscreen);
    //	UIP_Sysconfig = new CUI_Sysconfig;
    return 0;
}

int postAction () {  // Deinit functions
    
    SDL_Quit();
    
    intlist *mod;
    OutputDevice *m;
    midiInDevice *mi;
    char blah[256];
    int i;
    char name[256];
    char val[256];
    char tt[256];
    
    chdir(cur_dir);
    for (i=0;i<MAX_MIDI_OUTS;i++) {
        sprintf(name,"open_out_device_%d",i);
        zt_globals.Config->remove(&name[0]);
    }
    for (i=0;i<MAX_MIDI_INS;i++) {
        sprintf(name,"open_in_device_%d",i);
        zt_globals.Config->remove(&name[0]);
    }
    //tt = NULL;
    for (int j=0;j<MidiOut->numOuputDevices;j++) {
        
        m = MidiOut->outputDevices[j];
        //		if(tt != NULL)
        //			free(tt);
        encode(m->szName, tt);
        //        zt_globals.Config->set(&m->szName[0], (m->alias != NULL)?m->alias:"",0);
        sprintf(blah,"latency_%s",tt);
        sprintf(val,"%d",m->delay_ticks);
        zt_globals.Config->set(&blah[0], val);
        sprintf(blah,"bank_%s",tt);
#warning TODO
//        zt_globals.Config->set(&blah[0], ( ((MidiOutputDevice*)m )->reverse_bank_select)?"Yes":"No");
        if(m->alias != NULL && strlen(m->alias) > 1) {
            sprintf(blah,"alias_%s",tt);
            zt_globals.Config->set(&blah[0], m->alias);
        }
    }
    //if(tt != NULL)
    //	free(tt);
    if (MidiOut && MidiIn) {
        if (zt_globals.auto_open_midi) {
            i=0;
            mod = MidiOut->devlist_head;
            while(mod) {
                m = MidiOut->outputDevices[mod->key];
                sprintf(name,"open_out_device_%d",i);
                zt_globals.Config->set(&name[0], m->szName,0);
                mod = mod->next; i++;
            }
            i=0;
            mod = MidiIn->devlist_head;
            while(mod) {
                mi = MidiIn->midiInDev[mod->key];
                sprintf(name,"open_in_device_%d",i);
                zt_globals.Config->set(&name[0], mi->szName,0);
                mod = mod->next; i++;
            }
        }
    }
    zt_globals.save();
    
    if (cur_dir)
        free(cur_dir);
    if (ztPlayer)
        ztPlayer->stop();
    if (MidiOut)
        MidiOut->panic();
    if (clipboard)
        delete clipboard;
#warning TODO
//    KillTimer(NULL,keytimer);
    if (UI_Toolbar) {
#ifdef DEBUG
        playbuff1_bg = NULL;
        playbuff2_bg = NULL;
#endif
        delete UI_Toolbar;
    }
    delete InstEditorUI;
    delete UI; UI=NULL;
    delete UIP_About;
    delete UIP_InstEditor;
    delete UIP_Logoscreen;
    delete UIP_Loadscreen;
    delete UIP_Savescreen;
    delete UIP_LoadMsg;
    delete UIP_SaveMsg;
    delete UIP_Ordereditor;
    delete UIP_Playsong;
    delete UIP_Songconfig;
    delete UIP_Sysconfig;
    delete UIP_Patterneditor;
    delete UIP_PEParms;
    delete UIP_SliderInput;
    delete UIP_NewSong;
    delete UIP_RUSure;
    delete UIP_Help;
    delete UIP_Config;
    delete UIP_PEVol;
    delete UIP_PENature;
    delete UIP_SongDuration;
    delete UIP_SongMessage;
    delete UIP_Arpeggioeditor;
    delete UIP_Midimacroeditor;
    delete ztPlayer;
    delete MidiIn;
    delete MidiOut;
    delete song;
    delete CurrentSkin;
    delete bmToolbarRepeater;
    
    return 0;
}

void update_lights(ZTDrawable *S) {
    if (ztPlayer->playing) {
        if (need_update_lights) {
            need_update_lights = 0;
            if (S->lock() == 0) {
                int addy = CONSOLE_HEIGHT-480;
                for (int x=0;x<4;x++) {
                    TColor c;
                    if (x==light_pos && x==0) {
                        c = COLORS.LCDHigh;
                    } else if (x==light_pos){
                        c = COLORS.LCDMid;
                    } else {
                        c = COLORS.LCDLow;
                    }
                    S->fillRect(2+(x*8),471+addy,2+(x*8)+6,477+addy,c);
                }
                S->unlock();
                screenmanager.Update(2,471+addy,2+(3*8)+6+1,478+addy);
            }
        }
    }
}

#warning TODO

//VOID CALLBACK TP_Keyboard_Repeat(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
//    if (keyID) {
//        if ((!key_jazz) || jazz[keyID].note==0x80)
//            if (!(jazz[keyID].note!=0x80 && cur_state==STATE_IEDIT))
//                if (!bDontKeyRepeat && keyID != DIK_8 && keyID != DIK_4)
//                    Keys.insert(keyID,KS_LAST_STATE);
//        if (jazz[keyID].note != 0x80 && !key_jazz) {
//            if (cur_state!=STATE_IEDIT) {
//                if(keyID == DIK_8)
//                    MidiOut->panic();
//                else
//                    MidiOut->noteOff(song->instruments[cur_inst]->midi_device,jazz[keyID].note,jazz[keyID].chan,0x0,0);
//                jazz[keyID].note = 0x80;
//            }
//        }
//    }
//    if (keywait == 1) {
//        keywait = 0;
//        KillTimer(NULL,keytimer);
//        keytimer = SetTimer(NULL,keytimer,zt_globals.key_repeat_time,(TIMERPROC)TP_Keyboard_Repeat);
//    }
//}


/* MAIN */

void keyhandler(SDL_KeyboardEvent *e) {
    SDLKey id = e->keysym.sym;
    char actual_ch = e->keysym.unicode & 0x7F;
    int pressed = (e->state == SDL_PRESSED);
    if (id == DIK_NUMPADENTER)
        id=SDLK_RETURN;
    if (id != DIK_LALT && id != DIK_RALT && id != DIK_RCONTROL && id != DIK_LCONTROL && id != DIK_LSHIFT && id != DIK_RSHIFT) {
        if (zclear_flag)
            zclear_presscount = 0;
        else
            zclear_flag = 0;
        
        if (!pressed) {
            if (id == last_keyjazz) {
                last_cmd_keyjazz = 0;
                last_keyjazz = 0;
            }
            if (id == keyID) {
                keyID = 0;
#warning TODO

//                KillTimer(NULL, keytimer);
            }
            if (jazz[id].note != 0x80) {
                if(id == DIK_8)
                    MidiOut->panic();
                else
                    MidiOut->noteOff(song->instruments[cur_inst]->midi_device,jazz[id].note,jazz[id].chan,0x0,0);
                jazz[id].note = 0x80;
            }
        } else  {
            Keys.insert(id,e->keysym.mod,actual_ch);
            if (!(id == DIK_RETURN && e->keysym.mod&KMOD_ALT)) {
#warning TODO
//                keytimer = SetTimer(NULL,keytimer,zt_globals.key_wait_time,(TIMERPROC)TP_Keyboard_Repeat);
                keywait = 1;
                keyID = id;
            }
        }
    }
}

void mousemotionhandler(SDL_MouseMotionEvent *e) {
    LastX = e->x;
    LastY = e->y;
}

void mouseupbuttonhandler(SDL_MouseButtonEvent *e) {
    switch(e->button) {
        case SDL_BUTTON_LEFT:
            Keys.insert(DIK_MOUSE_1_OFF);
            bMouseIsDown = false;
            break;
        case SDL_BUTTON_RIGHT:
            Keys.insert(DIK_MOUSE_2_OFF);
            break;
    }
    MousePressX = LastX;
    MousePressY = LastY;
}

void mousedownbuttonhandler(SDL_MouseButtonEvent *e) {
    switch(e->button) {
        case SDL_BUTTON_LEFT:
            Keys.insert(DIK_MOUSE_1_ON);
            bMouseIsDown = true;
            break;
        case SDL_BUTTON_RIGHT:
            Keys.insert(DIK_MOUSE_2_ON);
            break;
    }
    MousePressX = LastX;
    MousePressY = LastY;
}


int action(ZTScreen *S) {
    
    if (cur_state!=STATE_LOGO)
    {
        if (!modal)
            UI_Toolbar->update();
        
        global_keys(S);
    }
    
    if (doredraw)
    {
        if (cur_state != STATE_LOGO)
            redrawscreen(S);
        
        fixmouse++;
        Keys.flush();
        doredraw=0;
        UI->full_refresh();
        need_refresh++;
    }
    
    if (status_change)
    {
        update_status(S);
    }
    
    
    //  if (PopupWindow)
    //      PopupWindow->update(K);
    if (!window_stack.isempty())
        window_stack.update();
    else
        ActivePage->update();
    
    
    
#ifdef DEBUG
    playbuff1_bg->setvalue(ztPlayer->play_buffer[0]->size);
    playbuff2_bg->setvalue(ztPlayer->play_buffer[1]->size);
    keybuff_bg->setvalue(Keys.cursize);
#endif

    if (need_popup_refresh)
        //      if (PopupWindow)
        //          PopupWindow->draw(S);
        if (!window_stack.isempty())
            window_stack.draw(S);
        else
            need_popup_refresh = 0;
    
    
    if (need_refresh) {
        fixmouse++;
        if (need_update)
            ActivePage->update();
        if (clear_popup)
        {
            if (S->lock()==0)
            {
                S->fillRect(col(1),row(12),CONSOLE_WIDTH,424,COLORS.Background);
                S->unlock();
                screenmanager.UpdateAll();
            }
            
            clear_popup = 0;
        }

        ActivePage->draw(S);
        
        if (!window_stack.isempty())
        {
            window_stack.draw(S);
        }
        
        if (cur_state!=STATE_LOGO)
        {
            UI_Toolbar->draw(S);
        }
    }
    
    if (do_exit)
    {
        return 1;
    }
    
    if (screenmanager.NeedRefresh())
    {
        need_update_lights = 1;
    }
    
    update_lights(S);
    
    screenmanager.Refresh(S);
    
    if (updated)
    {
        updated = 0;
    }
    
    return 0;
}

#ifndef main
#define main SDL_main
#endif

extern int SDL_Window;

SDL_Surface *initSDL(void)
{
    char errstr[2048];
    
    //  char *errstr;
    if(zt_globals.default_directory[0] != '\0')
    {
#warning TODO
//        SetCurrentDirectory((LPCTSTR)zt_globals.default_directory);
    }
    
    cur_dir = (LPSTR)malloc(256);
#warning TODO
    
//    GetCurrentDirectory(256,(LPSTR)cur_dir);
    
    if (zt_globals.load())
    {
#warning TODO

//        MessageBox(NULL,"Fatal: Unable to load zt.conf","zt: error",MB_OK | MB_ICONERROR);
        return NULL;
    }
    
    /*
     if (zcmp(Config.get("fullscreen"),"yes")) {
     //FULLSCREEN=1;
     zt_globals.full_screen = 1;
     } else {
     //FULLSCREEN=0;
     zt_globals.full_screen = 0;
     }
     */
    CurrentSkin = new Skin;
    if (! CurrentSkin->load(zt_globals.skin) ) {
        
        return NULL;
    }
    
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) {
        sprintf(errstr,"Could not initialize SDL: %s.\n", SDL_GetError());
#warning TODO
//        MessageBox(NULL,errstr,"Error",MB_ICONERROR | MB_OK);
        return(NULL);
    }
    
    Uint32 flags = SDL_SWSURFACE;//SDL_HWSURFACE;
    if (zt_globals.full_screen) {
        flags = SDL_FULLSCREEN; //SDL_SWSURFACE | SDL_FULLSCREEN;
        bIsFullscreen = true;
    }
    
    SDL_Surface *screen = SDL_SetVideoMode(CONSOLE_WIDTH, CONSOLE_HEIGHT, CONSOLE_DEPTH, flags);
    
    if ( screen == NULL ) {
        sprintf(errstr, "Couldn't set 640x480x32 video mode: %s\n",
                SDL_GetError());
#warning TODO
//        MessageBox(NULL,errstr,"Error",MB_ICONERROR | MB_OK);
        return NULL;
    } else {
        if (screen->w != CONSOLE_WIDTH || screen->h != CONSOLE_HEIGHT || screen->format->BitsPerPixel != CONSOLE_DEPTH) {
            sprintf(errstr, "Set mode, but it wasn't what i wanted: %s\n", SDL_GetError());
#warning TODO
//            MessageBox(NULL,errstr,"Error",MB_ICONERROR | MB_OK);
            return NULL;
        }
        
    }
    
    //    char str[80];
    //    istream *is;
    /*
     Skin = newResourceStream(skinfile);
     if (!Skin) {
     sprintf(str,"Fatal: Could not load skin resource: %s\n",skinfile);
     MessageBox(NULL,str,"zt: error",MB_OK | MB_ICONERROR );
     return -1;
     }
     is = Skin->getStream("font.fnt");
     Skin->freeStream(is);
     if (!S->isModeAvailable(CONSOLE_WIDTH,CONSOLE_HEIGHT)) {
     sprintf(str,"Fatal: Screen mode (%dx%dx16) is not support by your gfx card",CONSOLE_WIDTH, CONSOLE_HEIGHT);
     MessageBox(NULL,str,"zt: error",MB_OK | MB_ICONERROR);
     return -1;
     }
     */
    //    BM_Cache = new BitmapCache(*Skin);
    /*
     if (font_load(".\\skin\\font.fnt")) {
     sprintf(str,"Fatal: Could not load font.fnt from %s\n",skinfile);
     MessageBox(NULL,str,"zt: error",MB_OK | MB_ICONERROR );
     return NULL;
     }
     */
    
    MidiOut = new midiOut;
    MidiIn  = new midiIn;
    setup_midi();
    
    
    UI = new UserInterface;
    UI_Toolbar = new UserInterface;
    UI_Toolbar->dontmess = 1;
    init_edit_cols();
    clipboard = new CClipboard;
    
    song = new zt_module(zt_globals.default_tpb, zt_globals.default_bpm);
    
    ztPlayer = new player(1,zt_globals.prebuffer_rows, song); // 1ms resolution;
    
    ///////////////////////////////////////////////////////////////////////
    // Here we set the highlight and lowlight according to either specific
    // zt.conf parameters or ticks per beat (default)
    
    //    if(zt_globals.highlight_increment)
    //      highlight_count = zt_globals.highlight_increment;
    //    else
    //      highlight_count = song->tpb;
    //    if(zt_globals.lowlight_increment)
    //      lowlight_count = zt_globals.lowlight_increment;
    //    else
    //      lowlight_count = song->tpb >> 1 / song->tpb / 2;
    
    ////////////////////////////////////////////////////////////////////////
    
    for(int i=0;i<512;i++) jazz[i].note=0x80;
    
    // This loads the icon from the resource and attaches it to the main window
    
#warning TODO
    
//    HICON icon=(HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ZTICON),IMAGE_ICON,0,0,0);
//    HWND w = FindWindow("zt","zt");
//    SDL_WM_SetCaption("zt","zt");
//    SetClassLong(w, GCL_HICON, (LONG)icon);
    
    
    UIP_About = new CUI_About;
    UIP_InstEditor = new CUI_InstEditor;
    UIP_Loadscreen = new CUI_Loadscreen;
    UIP_Logoscreen = new CUI_Logoscreen;
    UIP_Savescreen = new CUI_Savescreen;
    UIP_LoadMsg = new CUI_LoadMsg;
    UIP_SaveMsg = new CUI_SaveMsg;
    UIP_Ordereditor = new CUI_Ordereditor;
    UIP_Playsong = new CUI_Playsong;
    UIP_Songconfig = new CUI_Songconfig;
    UIP_Sysconfig = new CUI_Sysconfig;
    // this guy has been moved to initGFX because the MIDI out devices are not setup yet
    UIP_Config = new CUI_Config;
    UIP_Patterneditor = new CUI_Patterneditor;
    UIP_PEParms = new CUI_PEParms;
    UIP_PEVol = new CUI_PEVol;
    UIP_PENature = new CUI_PENature;
    UIP_SliderInput = new CUI_SliderInput;
    UIP_NewSong = new CUI_NewSong;
    UIP_SongMessage = new CUI_SongMessage;
    UIP_Arpeggioeditor = new CUI_Arpeggioeditor;
    UIP_Midimacroeditor = new CUI_Midimacroeditor;
    UIP_RUSure = new CUI_RUSure;
    UIP_Help = new CUI_Help;
    UIP_SongDuration = new CUI_SongDuration;
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
    
    return screen;
    
}
#ifdef _ENABLE_AUDIO

void audio_mixer(void *udata, Uint8 *stream, int len) {
    MidiOut->MixAudio(udata,stream,len);
}

#endif


// did i mention how much i love SDL?
SDL_Surface *scrn_surface ;
ZTScreen *thescreen = NULL;
int main(int argc, char *argv[]) {
    char *w;
    
    // Get the zt directory and store it globally
    
    if(argc > 1)
    {
        if(argv[1] != NULL && argv[1][0] != '\0')
        {
            // store current directory
#warning TODO
//            GetCurrentDirectory(1024,zt_filename);
            strcat(zt_filename,"\\");
            strcat(zt_filename,argv[1]);
        }
    }
    if(strstr(argv[0],"\\"))
    {
        for(w = argv[0] + strlen(argv[0]); w != argv[0] && *w != '\\'; w--);
        if(w != argv[0])
        {
            *w = '\0';
            zt_directory = strdup(argv[0]);
#warning TODO
//            SetCurrentDirectory(argv[0]);
            *w = '\\';
        }
    }
    else zt_directory = strdup("");
    
    doredraw++;
    
    /*
     FXMLParser fxml;
     FXMLElement *xmlconfig;
     
     fxml.nParser = USE_PARSER_FXML;
     xmlconfig = fxml.ParseFile("config.xml");
     if (xmlconfig) {
     std::string a = xmlconfig->getValueFromPath("general.skin");
     a = a;
     }
     delete xmlconfig;
     */
    scrn_surface = initSDL();
    if(scrn_surface == NULL) {
        
        // Error opening the skin !
        exit(1);
    }
    ZTScreen scrn(scrn_surface,false);
    thescreen = &scrn;
    if (scrn_surface != NULL) {
        scrn.surface = scrn_surface;
        S = RealScreen = &scrn;
        initGFX();
        if(argc > 0) {
            song->load(argv[1]);
        }
        else
            if (zt_globals.autoload_ztfile)
                if (strlen(zt_globals.autoload_ztfile_filename))
                    song->load(zt_globals.autoload_ztfile_filename);
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
        SDL_EventState(SDL_ACTIVEEVENT, SDL_ENABLE);
        
        
#ifdef _ENABLE_AUDIO
        
        SDL_AudioSpec wanted;
        
        /* Set the audio format */
        wanted.freq = 44100 ;
        wanted.format = AUDIO_S16;
        wanted.channels = 2;    /* 1 = mono, 2 = stereo */
        wanted.samples = 2048;  /* Good low-latency value for callback */
        wanted.callback = audio_mixer;
        wanted.userdata = NULL;
        
        
        /* Open the audio device, forcing the desired format */
        if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
            fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
            return(-1);
        }
        //    int ret = Mix_OpenAudio(wanted.freq, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, wanted.samples);
        SDL_PauseAudio(0);
        
#endif
        
        
        while (!action(&scrn))
        {
            SDL_Delay(1);
            
            SDL_Event e;
            
            while (SDL_PollEvent(&e))
            {  /* Loop until there are no events left on the queue */
                switch(e.type)
                {  /* Process the appropiate event type */
                    case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
                    case SDL_KEYUP:
                        keyhandler(&e.key);
                        break;
                    case SDL_MOUSEMOTION:
                        mousemotionhandler(&e.motion);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        mousedownbuttonhandler(&e.button);
                        break;
                    case SDL_MOUSEBUTTONUP:
                        mouseupbuttonhandler(&e.button);
                        break;
                    case SDL_QUIT:
                        quit();
                        break;
                    case SDL_ACTIVEEVENT:
                    {
                        int p = e.active.state;
                        switch(p) {
                            case SDL_APPMOUSEFOCUS:
                                if (!bIsFullscreen)
                                    break;
                            case SDL_APPINPUTFOCUS:
                            case SDL_APPACTIVE:
                                //SDL_WM_SetCaption(sr,sr);
                                //if (e.active.gain) {
                                if (UI) UI->full_refresh();
                                if (UI_Toolbar) UI_Toolbar->full_refresh();
                                doredraw++;
                                need_refresh++;
                                screenmanager.UpdateAll();
                                //}
                                break;
                        }
                    }
                        break;
                    case SDL_SYSWMEVENT:
                        //SDL_Delay(1);//e.syswm;
#warning TODO
                        
//                        switch(e.syswm.msg)
//                        {
//                            case WM_SETFOCUS:
                                if (UI) UI->full_refresh();
                                if (UI_Toolbar) UI_Toolbar->full_refresh();
                                doredraw++;
                                need_refresh++;
                                screenmanager.UpdateAll();
//                                break;
//                        }
                        
                        break;
                    default: /* Report an unhandled event */
                        break;
                }
                
            }    //      SDL_PumpEvents(); // Dont need this if we are using SDL_PollEvents from the action() loop
        }
        
#ifdef _ENABLE_AUDIO
        //    Mix_CloseAudio();
        SDL_CloseAudio();
#endif
        
        SDL_FreeSurface(scrn_surface);
    }
    
    postAction();
    
    SDL_Quit();
    
    return 0;
}


static int do_attempt_fullscreen_toggle(SDL_Surface **surface, Uint32 *flags)
{
    long framesize = 0;
    void *pixels = NULL;
    SDL_Rect clip;
    Uint32 tmpflags = 0;
    int w = 0;
    int h = 0;
    int bpp = 0;
    int grabmouse = (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON);
    int showmouse = SDL_ShowCursor(-1);
    
#ifdef BROKEN
    SDL_Color *palette = NULL;
    int ncolors = 0;
#endif
    
    if ( (!surface) || (!(*surface)) )  /* don't try if there's no surface. */
    {
        return(0);
    } /* if */
    
    if (SDL_WM_ToggleFullScreen(*surface))
    {
        if (flags)
            *flags ^= SDL_FULLSCREEN;
        return(1);
    } /* if */
    
    if ( !(SDL_GetVideoInfo()->wm_available) )
    {
        return(0);
    } /* if */
    
    tmpflags = (*surface)->flags;
    w = (*surface)->w;
    h = (*surface)->h;
    bpp = (*surface)->format->BitsPerPixel;
    
    if (flags == NULL)  /* use the surface's flags. */
        flags = &tmpflags;
    
    SDL_GetClipRect(*surface, &clip);
    
    /* save the contents of the screen. */
#ifdef BROKEN
    if ( (!(tmpflags & SDL_OPENGL)) && (!(tmpflags & SDL_OPENGLBLIT)) )
    {
        framesize = (w * h) * ((*surface)->format->BytesPerPixel);
        pixels = malloc(framesize);
        if (pixels == NULL)
            return(0);
        memcpy(pixels, (*surface)->pixels, framesize);
    } /* if */
    
    if ((*surface)->format->palette != NULL)
    {
        ncolors = (*surface)->format->palette->ncolors;
        palette = malloc(ncolors * sizeof (SDL_Color));
        if (palette == NULL)
        {
            free(pixels);
            return(0);
        } /* if */
        memcpy(palette, (*surface)->format->palette->colors,
               ncolors * sizeof (SDL_Color));
    } /* if */
#endif
    
    if (grabmouse)
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    
    SDL_ShowCursor(1);
    
    if ((*flags) & SDL_FULLSCREEN) {
        *flags = SDL_HWSURFACE;
        bIsFullscreen = false;
        zt_globals.full_screen = false;
    } else {
        *flags = SDL_SWSURFACE | SDL_FULLSCREEN;
        bIsFullscreen = true;
        zt_globals.full_screen = true;
    }
    
    SDL_Surface *surf = SDL_SetVideoMode(w, h, bpp, (*flags));
    *surface = surf;
    
    if (!(*surface))  /* yikes! Try to put it back as it was... */
    {
        //        *surface = SDL_SetVideoMode(w, h, bpp, tmpflags);
        if (*surface == NULL)  /* completely screwed. */
        {
#ifdef BROKEN
            if (pixels != NULL)
                free(pixels);
            if (palette != NULL)
                free(palette);
#endif
            return(0);
        } /* if */
    } /* if */
    
    /* Unfortunately, you lose your OpenGL image until the next frame... */
    
#ifdef BROKEN
    if (pixels != NULL)
    {
        memcpy((*surface)->pixels, pixels, framesize);
        free(pixels);
    } /* if */
    if (palette != NULL)
    {
        /* !!! FIXME : No idea if that flags param is right. */
        SDL_SetPalette(*surface, SDL_LOGPAL, palette, 0, ncolors);
        free(palette);
    } /* if */
#endif
    
    SDL_SetClipRect(*surface, &clip);
    
    if (grabmouse)
        SDL_WM_GrabInput(SDL_GRAB_ON);
    
    SDL_ShowCursor(showmouse);
    
    return(1);
} /* attempt_fullscreen_toggle */
int baba=0;

int attempt_fullscreen_toggle() {
    int r;
    if (baba==1) {
        baba=0;
        return 0;
    }
    baba++;
    r = do_attempt_fullscreen_toggle( &scrn_surface, NULL );
    thescreen->surface = scrn_surface; 
    need_refresh++;
    doredraw++;
    return r;
}
