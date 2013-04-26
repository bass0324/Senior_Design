#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include <pthread.h>
#include <stdio.h>
#include "p2p_test.h"

int value;
pthread_mutex_t mutexVALUE;

SDL_Surface *image = NULL;
pthread_mutex_t mutexIMAGE;

void takeScreenshot(void) {
    system("adb shell screencap -p /sdcard/dump1.png; adb shell cp /sdcard/dump1.png /sdcard/dump2.png");
}

void transferScreenshot(void) {
    system("adb pull /sdcard/dump2.png dump2.png");
}

int getHeight() {
    FILE* file;
    char buffer[256];
    int value;
    if((file = popen("adb shell dumpsys window | grep cur= | awk -F cur= '{print $NF}' | awk -F ' ' '{print $1}' | awk -F 'x' '{print $NF}'", "r"))==NULL)
    {
        fprintf(stderr, "can't get size\n");
        return 0;
    }
    while(fgets(buffer, sizeof(buffer), file) != 0)
    {
	value = atoi(buffer);
    }
    
    pclose(file);

    return value;
}

void *take_screen(void *a) {
    takeScreenshot();
}

void *get_screen(void *d) {
    transferScreenshot();
}

void *get_height(void *e) {
    pthread_mutex_lock (&mutexVALUE);
    value = getHeight();
    pthread_mutex_unlock(&mutexVALUE);
}

void *load_image(void *f) {
    pthread_mutex_lock (&mutexIMAGE);
    image = IMG_Load("dump2.png");
    pthread_mutex_unlock(&mutexIMAGE);
}

int doSDL(void) {
    SDL_Surface *screen = NULL;
    SDL_Surface *rot = NULL;
    const SDL_VideoInfo *videoInfo = NULL;
    double scale;
    int prevValue = 0;
    int rc = 0;

    pthread_t thread[7];
    void *status[7];

    pthread_mutex_init(&mutexVALUE, NULL); 
    pthread_mutex_init(&mutexIMAGE, NULL);

    pthread_create(&thread[0], NULL, take_screen, NULL);

    /*-----------------------------------------------------------------*/

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init failed - %s\n", SDL_GetError());
        return 1;
    }

    /*-----------------------------------------------------------------*/

    videoInfo = SDL_GetVideoInfo();

    if (videoInfo == 0)
    {
        fprintf(stderr, "SDL_GetVideoInfo failed - %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /*-----------------------------------------------------------------*/

    rc = pthread_join(thread[0], &status[0]);

    pthread_create(&thread[1], NULL, get_screen, NULL);

    pthread_create(&thread[0], NULL, take_screen, NULL);

    rc = pthread_join(thread[1], &status[1]);

    pthread_create(&thread[2], NULL, load_image, NULL);
    
    rc = pthread_join(thread[0], &status[0]);

    /*-----------------------------------------------------------------*/
while(1){
    pthread_create(&thread[1], NULL, get_screen, NULL);

    pthread_create(&thread[0], NULL, take_screen, NULL);

    pthread_create(&thread[3], NULL, get_height, NULL);

    rc = pthread_join(thread[3], &status[3]);

    rc = pthread_join(thread[1], &status[1]);

    pthread_create(&thread[2], NULL, load_image, NULL);

    rc = pthread_join(thread[2], &status[2]);

    if (value != image->h) {
        //scale = (image->w)/(videoInfo->current_w);
        scale = 1;
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->h)*scale,
		                      (image->w)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 90, scale, SMOOTHING_ON );
    }

    else if (value == image->h) {
	//scale = (image->h)/(videoInfo->current_h);
        scale = 0.5;
	if (prevValue != image->h) {
	    screen = SDL_SetVideoMode((image->w)*scale,
		                      (image->h)*scale,
		                      videoInfo->vfmt->BitsPerPixel,
		                      SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
        rot = rotozoomSurface( image, 0, scale, SMOOTHING_ON );
    }

    SDL_FreeSurface(image);

    SDL_BlitSurface(rot, NULL, screen, 0);

    //SDL_Flip(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    SDL_FreeSurface(rot);

    prevValue = value;

    rc = pthread_join(thread[0], &status[0]);
}

    SDL_Quit();

    return 0;
}

/************  WiFi Direct Stuff Needed to Work  ***********/

char *naming_wpsinfo(int wps_info)
{
	switch(wps_info)
	{
		case P2P_NO_WPSINFO: return ("P2P_NO_WPSINFO");
		case P2P_GOT_WPSINFO_PEER_DISPLAY_PIN: return ("P2P_GOT_WPSINFO_PEER_DISPLAY_PIN");
		case P2P_GOT_WPSINFO_SELF_DISPLAY_PIN: return ("P2P_GOT_WPSINFO_SELF_DISPLAY_PIN");
		case P2P_GOT_WPSINFO_PBC: return ("P2P_GOT_WPSINFO_PBC");
		default: return ("UI unknown failed");
	}
}

char *naming_role(int role)
{
	switch(role)
	{
		case P2P_ROLE_DISABLE: return ("P2P_ROLE_DISABLE");
		case P2P_ROLE_DEVICE: return ("P2P_ROLE_DEVICE");
		case P2P_ROLE_CLIENT: return ("P2P_ROLE_CLIENT");
		case P2P_ROLE_GO: return ("P2P_ROLE_GO");
		default: return ("UI unknown failed");
	}
}

char *naming_status(int status)
{
	switch(status)
	{
		case P2P_STATE_NONE: return ("P2P_STATE_NONE");
		case P2P_STATE_IDLE: return ("P2P_STATE_IDLE");
		case P2P_STATE_LISTEN: return ("P2P_STATE_LISTEN");
		case P2P_STATE_SCAN: return ("P2P_STATE_SCAN");
		case P2P_STATE_FIND_PHASE_LISTEN: return ("P2P_STATE_FIND_PHASE_LISTEN");
		case P2P_STATE_FIND_PHASE_SEARCH: return ("P2P_STATE_FIND_PHASE_SEARCH");
		case P2P_STATE_TX_PROVISION_DIS_REQ: return ("P2P_STATE_TX_PROVISION_DIS_REQ");
		case P2P_STATE_RX_PROVISION_DIS_RSP: return ("P2P_STATE_RX_PROVISION_DIS_RSP");
		case P2P_STATE_RX_PROVISION_DIS_REQ: return ("P2P_STATE_RX_PROVISION_DIS_REQ");
		case P2P_STATE_GONEGO_ING: return ("P2P_STATE_GONEGO_ING");
		case P2P_STATE_GONEGO_OK: return ("P2P_STATE_GONEGO_OK");
		case P2P_STATE_GONEGO_FAIL: return ("P2P_STATE_GONEGO_FAIL");
		case P2P_STATE_RECV_INVITE_REQ: return ("P2P_STATE_RECV_INVITE_REQ");
		case P2P_STATE_PROVISIONING_ING: return ("P2P_STATE_PROVISIONING_ING");
		case P2P_STATE_PROVISIONING_DONE: return ("P2P_STATE_PROVISIONING_DONE");
		default: return ("UI unknown failed");
	}
}

char* naming_enable(int enable)
{
	switch(enable)
	{
		case  P2P_ROLE_DISABLE: return ("[Disabled]");
		case  P2P_ROLE_DEVICE: return ("[Enable/Device]");
		case  P2P_ROLE_CLIENT: return ("[Enable/Client]");
		case  P2P_ROLE_GO: return ("[Enable/GO]");
		default: return ("UI unknown failed");
	}
}

void init_p2p(struct p2p *p)
{
	strcpy( p->ifname, "wlan0" );
	p->enable = P2P_ROLE_DISABLE;
	p->res = 1;
	p->res_go = 1;
	p->status = P2P_STATE_NONE;
	p->intent = 1;
	p->wps_info = 0;
	p->wpsing = 0;
	p->pin = 12345670;
	p->role = P2P_ROLE_DISABLE;
	p->listen_ch = 11;
	strcpy( p->peer_devaddr, "00:00:00:00:00:00" );
	p->p2p_get = 0;
	memset( p->print_line, 0x00, CMD_SZ);
	p->have_p2p_dev = 0;
	p->count_line = 0;
	strcpy( p->peer_ifaddr, "00:00:00:00:00:00" );
	memset( p->cmd, 0x00, CMD_SZ);
	p->wpa_open=0;
	p->ap_open=0;
	strcpy(p->ok_msg, "WiFi Direct handshake done" );
	strcpy(p->redo_msg, "Re-do GO handshake" );
	strcpy(p->fail_msg, "GO handshake unsuccessful" );
	strcpy(p->nego_msg, "Start P2P negotiation" );
	strcpy(p->wpa_conf, "./wpa_0_8.conf" );
	strcpy(p->wpa_path, "./wpa_supplicant" );
	strcpy(p->wpacli_path, "./wpa_cli" );
	strcpy(p->ap_conf, "./p2p_hostapd.conf" );
	strcpy(p->ap_path, "./hostapd" );
	strcpy(p->apcli_path, "./hostapd_cli" );
	strcpy(p->scan_msg, "Device haven't enable p2p functionalities" );
	
}

void rename_intf(struct p2p *p)
{
	FILE *pfin = NULL;
	FILE *pfout = NULL;
	
	pfin = fopen( p->ap_conf, "r" );
	pfout = fopen( "./p2p_hostapd_temp.conf", "w" );
	
	if ( pfin )
	{
		while( !feof( pfin ) ){
			memset(p->parse, 0x00, CMD_SZ);
			fgets(p->parse, CMD_SZ, pfin);
			
			if(strncmp(p->parse, "interface=", 10) == 0)
			{
				memset(p->parse, 0x00, CMD_SZ);
				sprintf( p->parse, "interface=%s\n", p->ifname );
				fputs( p->parse, pfout );
			}
			else
				fputs(p->parse, pfout);
		}
	}

	fclose( pfout );
	
	system( "rm -rf ./p2p_hostapd.conf" );
	system( "mv ./p2p_hostapd_temp.conf ./p2p_hostapd.conf" );
	
	return;
}

/********  End WiFi Direct Stuff  **************/

int doWifiDirect(void) {
    char	peerifa[40] = { 0x00 };
	char	scan[CMD_SZ];	
	struct p2p p2pstruct;
	struct p2p *p=NULL;

	p = &p2pstruct;	
	if( p != NULL)
	{
		memset(p, 0x00, sizeof(struct p2p));
		init_p2p(p);
	}

	strcpy(p->ifname, "wlan0" );
	
	/* Disable P2P functionalities at first*/
	p->enable=P2P_ROLE_DISABLE;
	p2p_enable(p);
	p2p_get_hostapd_conf(p);
	usleep(50000);
  
  	rename_intf(p);
  
	do
	{
		system("clear");

		memset( scan, 0x00, CMD_SZ );

		if( p->thread_trigger == THREAD_NONE )		//Active mode for user interface
		{
		
			system("clear"); 

			p->enable = 1;

			p2p_enable(p);

			p2p_scan(p);
			usleep(500000);

			system("clear");
			p2p_devaddr(p);

			system("echo \"discovering device\""); //Connecting to device
			char msg[CMD_SZ];
			memset( msg, 0x00, CMD_SZ );
			char dis_msg[CMD_SZ];
			memset( dis_msg, 0x00, CMD_SZ );
			char label_msg[CMD_SZ];
			memset( label_msg, 0x00, CMD_SZ );


			if(strncmp(p->peer_devaddr, "00:00:00:00:00:00", 17) == 0)
			{
				strcpy( msg, "Please insert peer P2P device at first" );			

				p2p_prov_disc_no_addr(p, msg);
				p->show_scan_result = 1;
			}
			else
			{
				strcpy( msg, "Please insert WPS configuration method ;[0]display, [1]keypad, [2]pbc, [3]label:\n" );
				strcpy( dis_msg, "Please insert PIN code displays on peer device screen:" );
				strcpy( label_msg, "Please insert PIN code displays on peer label:" );

				p2p_prov_disc(p, msg, dis_msg, label_msg);
			}

			system("echo \"connection completed\""); //Exit Program
			if( p->res == 0 )
				p->res = 1;
			if( p->res_go == 0 )
				p->res_go = 1;
			break;
			
		}
		else if( p->thread_trigger == THREAD_DEVICE )		//Passive mode for user interface
		{
			
			p->thread_trigger = THREAD_NONE ;
			
			if( strncmp(scan, "b", 1) == 0 )
			{
				p->wps_info=3;
				p2p_wpsinfo(p);
				
				p2p_status(p, 0);
				
				if(p->status != P2P_STATE_GONEGO_OK)
				{
					p2p_set_nego(p);
				}
				else
				{
					p2p_role(p,0);
										
					if( p->role == P2P_ROLE_CLIENT )
					{
						p2p_client_mode(p);
					}
					else if( p->role == P2P_ROLE_GO )
					{
						p2p_go_mode(p);
					}
				}
			}
			else if( strncmp(scan, "c", 1) == 0 )
			{
				p->wps_info=2;
				p2p_wpsinfo(p);
				
				p2p_status(p, 0);
							
				if(p->status != P2P_STATE_GONEGO_OK)
				{
					p2p_set_nego(p);
				}					
				else
				{
					p2p_role(p,0);
					p2p_ifaddr(p);
										
					if( p->role == P2P_ROLE_CLIENT )
					{
						p2p_client_mode(p);
					}
					else if( p->role == P2P_ROLE_GO )
					{
						p2p_go_mode(p);
					}
				}
			}
			else if( ('0' <= *scan ) && ( *scan <= '9') )
			{
				printf("In passive pin code\n");
				
				p->pin = atoi(scan);
				
				p->wps_info=1;
				p2p_wpsinfo(p);
				
				p2p_set_nego(p);
			}
		}
		else if( p->thread_trigger == THREAD_GO )		//Passive mode for user interface
		{
			
			p->thread_trigger = THREAD_NONE ;
			
			if( strncmp(scan, "b", 1) == 0 )
			{
				p->wps_info=3;
				p2p_wpsinfo(p);

			}
			else if( strncmp(scan, "c", 1) == 0 )
			{
				p->wps_info=2;
				p2p_wpsinfo(p);
			}
			else if( ('0' <= *scan ) && ( *scan <= '9') )
			{
				printf("In passive pin code\n");
				
				p->pin = atoi(scan);
				
				p->wps_info=1;
				p2p_wpsinfo(p);
			}
			
			p2p_go_mode(p);
			
		}
	}
	while( 1 );

	/* Disable P2P functionalities when exits*/
	p->enable= -1 ;
	p2p_enable(p);

	system( "rm -f ./supp_status.txt" );
	system( "rm -f ./temp.txt" );
	system( "rm -f ./scan.txt" );
	system( "rm -f ./peer.txt" );
	system( "rm -f ./status.txt" );
	system( "rm -f ./cm.txt" );
	
	return 0;
}
 
int main(int argc, char* argv[])
{
    doWifiDirect();
    system("adb connect 192.168.49.1");
    doSDL();
}
