# include "iGraphics.h"
# include <math.h>
# pragma comment(lib, "Winmm.lib")
# define MAX_BULLETS 10
# define MAX_BULLETS_ENEMY 10
# define MAX_AST 5
# define MAX_BOUL 5
# define MAX_DUST 50
# define MAX_LIVE 2 
# define MAX_SHIELD 1
# define MAX_ENEMY 2
# define MAX_BOSS 2
# define MAX_LEADERBOARD 5

    struct asteroids{
        double x, y, dx=5, dy=5;
        bool alive; 
        int height = 40,width = 40;
    };
    struct boulders{
        double x, y, dx=5, dy=5;
        bool alive;
        int height = 30,width = 30;
    };
    struct bullets{
        double x, y;
        bool bullet_active = false;
        int bullet_speed = 10;
    };
    struct dust{
        double x, y;
        int dy = 3;
    };  
    struct livespawn{
        double x, y;
        int dy = 10;
        bool active;
        int height = 40,width = 40;
    };
    struct shieldspawn{
        double x, y;
        int dy = 10;
        bool active;
        int height = 40,width = 40;

    };
    struct explosion{
        double x, y;
        bool exp;
    }explode;
    struct enemies{
        double x, y;
        double dx=5, dy=5;
        double health=3;
        int height = 50, width = 50;
        bool active;
        struct enemybullets{
            double x, y;
            bool bullet_active = false;
            int bullet_speed = 10;
        }enemybullet[MAX_BULLETS_ENEMY];
    };
    typedef struct {
        double x, y;
        double dx=5, dy=5;
        int health=4;
        int height = 60, width = 60;
        bool active;
        struct enemy_missile{
            double x, y;
            bool missile_active = false;
            double dx=5, dy=5;
            int missile_speed = 10;
            int height = 30, width = 30;
        }missile;
    }Boss;
    typedef struct {
        char name[100];
        int score;
    } Player;
    typedef struct {
        double x, y;
        int dy = 10, dx = 10;
        bool active;
        int height = 40,width = 40;
        int count = 0;
    }fire_rate;

    int screen_width = 840, screen_height = 770;

    struct asteroids asteroid[MAX_AST];
    struct boulders boulder[MAX_BOUL];
    struct bullets bullet[MAX_BULLETS];
    struct dust dust[MAX_DUST];
    struct livespawn live[MAX_LIVE];
    struct shieldspawn shield[MAX_SHIELD];
    struct enemies enemy[MAX_ENEMY];
    Player leaderboard[MAX_LEADERBOARD];
    fire_rate firerate;
    Boss boss[MAX_BOSS];
    // Player variables
    int player_x = screen_width/2, player_y = 50, player_width = 50, player_height = 50, player_speed = 15; 
    bool player_shield = false;

    // Game variables
    float diff_factor = 1;
    int score = 0, *Pscore = &score;
    int health = 100, game_state = 0, takename = 0, leaderboard_size = 0, *Phealth = &health, shield_counter = 0, time_enemybullet;
    int firemode = 1, scalingfactor_enemy = 0, scalingfactor_boss = 0, scalingfactor_missile = 0;
    char namebuffer[100] = {0};
    bool music = true;

    // Initialize bullets and asteroid
    void update_leaderboard();
    void initialize_game() {
        for (int i = 0; i < MAX_AST; i++) {
            asteroid[i].x = rand() % (screen_width - asteroid[i].width);
            asteroid[i].y = screen_height + rand() % 300;
        }
        for (int i = 0; i < MAX_BOUL; i++) {
            boulder[i].dy = boulder[i].dy;
            boulder[i].dx = boulder[i].dx*(i%2 == 0?-1:1);
            
            boulder[i].x = rand() % (screen_width - boulder[i].width);
            boulder[i].y = screen_height + rand() % 300;
        }
        for (int i = 0; i < MAX_DUST; i++) {
            dust[i].x = rand() % (screen_width);
            dust[i].y = screen_height + rand() % 300;
        }
        
        for (int i = 0; i < MAX_ENEMY; i++) {
            enemy[i].x = 100 + rand() % (screen_width-200);
            enemy[i].y = screen_height + rand() % 300;
            for(int j = 0; j < MAX_BULLETS_ENEMY; j++) {
                enemy[i].enemybullet[j].bullet_active = false;
            }
        }
        for(int i = 0; i < MAX_BOSS; i++) {
            boss[i].x = 100 + rand() % (screen_width-200);
            boss[i].y = screen_height + rand() % 300;
            boss[i].missile.missile_active = false;
        }
        for(int i = 0; i < MAX_LIVE; i++) {
            live[i].active = false;
        }
        for(int i = 0; i < MAX_SHIELD; i++) {
            shield[i].active = false;
        }
        update_leaderboard();
    }
    
    void gamereset(){
        game_state = 0;
        *Phealth = 100;
        *Pscore = 0;
        firemode = 1;
        player_shield = false;
        initialize_game();
        update_leaderboard();
    }

    bool check_collision(double x1, double y1, double w1, double h1, 
                        double x2, double y2, double w2, double h2) {
        if (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2) {
            return true; 
        }
        return false; 
    }

    bool buttoncheck(int x, int y, int h, int w, int mx, int my){
        if(mx > x && mx < x + w && my > y && my < y + h)
        return true;
        return false;
    }

    void update_leaderboard() {
        leaderboard_size = 0;
        FILE* file = fopen("leaderboard.txt", "r");
        if (file) {
            while (fscanf(file, "%s %d", leaderboard[leaderboard_size].name, 
                        &leaderboard[leaderboard_size].score) == 2) {
                leaderboard_size++;
                if (leaderboard_size >= MAX_LEADERBOARD) break; 
            }
            fclose(file); 
        }
        for (int i = 1; i < leaderboard_size; i++) {
            Player c = leaderboard[i];
            int j = i - 1;
            while (j >= 0 && leaderboard[j].score < c.score) {
                leaderboard[j + 1] = leaderboard[j];
                j--;
            }
            leaderboard[j + 1] = c;
        }
    }

    void update_bullets() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                bullet[i].y += bullet[i].bullet_speed;
                if (bullet[i].y > screen_height) {
                    bullet[i].bullet_active = false; 
                }
            }
            //printf("%d %d %d\n", i, bullet[i].x, bullet[i].y);
        }
    }
    void update_bullet_enemy() {
        for(int i = 0; i < MAX_ENEMY; i++) {
            for (int j = 0; j < MAX_BULLETS_ENEMY; j++) {
                if (enemy[i].enemybullet[j].bullet_active) {
                    enemy[i].enemybullet[j].y -= enemy[i].enemybullet[j].bullet_speed;
                    if (enemy[i].enemybullet[j].y < 0) {
                        enemy[i].enemybullet[j].bullet_active = false; 
                    }
                }
            }
        }
    }
    void update_enemy(){
        scalingfactor_enemy++;
        if(scalingfactor_enemy%2 == 0){
            for (int i = 0; i < MAX_ENEMY; i++) {
            if(enemy[i].y > (screen_height-120))enemy[i].y -= enemy[i].dy;
            if(enemy[i].x < player_x){
                enemy[i].x += 4;
            }
            else if (enemy[i].x > player_x) {
                enemy[i].x -= 4;
            }
            }
            if(enemy[0].x < 0)enemy[0].x = enemy[0].width/2;
            if(enemy[0].x + enemy[0].width + 40 > enemy[1].x) enemy[0].x -= 5;
            if(enemy[1].x < enemy[0].x + enemy[0].width + 40)enemy[1].x += 5;
            if(enemy[1].x > screen_width) enemy[1].x = screen_width - enemy[1].width/2;
        }        
    }
    void update_missile() {
        scalingfactor_missile++;
        for(int i = 0; i < MAX_BOSS; i++) {
            if(boss[i].missile.missile_active) {
                boss[i].missile.y -= boss[i].missile.dy;
                if(boss[i].missile.x < player_x){
                    boss[i].missile.x += 6;
                }
                else if (boss[i].x > player_x) {
                    boss[i].missile.x -= 6;
                }
            }
            if (boss[i].missile.y <= 0) {
                boss[i].missile.missile_active = false; 
            }   
            if(boss[i].missile.x < 0)boss[i].missile.x = boss[i].missile.width/2;
            if(boss[i].missile.x > screen_width)boss[i].missile.x = screen_width - boss[i].missile.width/2;            
            if(boss[i].missile.x == 0 && boss[i].missile.y == 0)boss[i].missile.missile_active = false;
        }
    }
    void update_asteroid() {
        for (int i = 0; i < MAX_AST; i++) {
            asteroid[i].y -= asteroid[i].dy;

            if (asteroid[i].y < 0) {
                asteroid[i].x = rand() % (screen_width - asteroid[i].width);
                asteroid[i].y = screen_height + rand() % 300;
            }
        }
    }
    void update_boulder() {
        for(int i = 0; i < MAX_BOUL; i++)
        {   
            boulder[i].x += boulder[i].dx;    
            boulder[i].y -= boulder[i].dy;    
            if(boulder[i].x < 0 || boulder[i].x + boulder[i].width > screen_width){
                boulder[i].dx = -boulder[i].dx;
            }
            if (boulder[i].y < 0) {
                boulder[i].x = rand() % (screen_width - boulder[i].width);
                boulder[i].y = screen_height + rand() % 300;
            }
        }
    }
    void update_dust() {
        for (int i = 0; i < MAX_DUST; i++) {
            dust[i].y -= dust[i].dy;
            if (dust[i].y < 0) {
                dust[i].x = rand() % (screen_width);
                dust[i].y = screen_height + rand() % 300;
            }
        }
    }
    void update_boss() {
        scalingfactor_boss++;
        if(scalingfactor_boss%2 == 0){
            for(int i = 0; i < MAX_BOSS; i++) {
                if(boss[i].y > (screen_height-220))boss[i].y -= boss[i].dy;
                if(boss[i].x < player_x){
                    boss[i].x += 6;
                }
                else if (boss[i].x > player_x) {
                    boss[i].x -= 6;
                }
            }
                if(boss[0].x < 0)boss[0].x = boss[0].width/2;
                //if(boss[0].x + boss[0].width + 40 > boss[1].x)boss[0].x -= 5;//(screen_width/2) - boss[0].width/2;
                if(boss[0].x > (screen_width/2) - boss[0].width/2)boss[0].x -= 5;//(screen_width/2) - boss[0].width/2;
                //if(boss[1].x < boss[0].x + boss[0].width + 40)boss[1].x += 5;//(screen_width/2) + boss[1].width/2;
                if(boss[1].x < (screen_width/2) + boss[1].width/2)boss[1].x += 5;//(screen_width/2) + boss[1].width/2;
                if(boss[1].x > screen_width)boss[1].x = screen_width - boss[1].width/2;
        }
    } 
    void update_live_spawn(){
        for(int i = 0; i<MAX_LIVE; i++){
            if(live[i].active){
                live[i].y -= live[i].dy;
            }
            if(live[i].y<0){
                live[i].active = false;
            }
        }
    }
    void update_firerate_spawn() {
        if(firerate.active) {
            firerate.y -= firerate.dy;
            firerate.x += firerate.dx;    
    
            if(firerate.x < 0 || firerate.x + firerate.width > screen_width){
                firerate.dx = -firerate.dx;
            }
        }
        if(firerate.y < 0) {
            firerate.active = false;
        }
    }
    void update_shield_spawn(){
        for(int i = 0; i<MAX_SHIELD; i++){
            if(shield[i].active){
                shield[i].y -= shield[i].dy;
            }
            if(shield[i].y<0){
                shield[i].active = false;
            }
        }
    }
    void fire_bullet() {
        if(firemode == 1) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x + player_width / 2;
                    bullet[i].y = player_y + player_height;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
        }
        else if(firemode == 2) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x;
                    bullet[i].y = player_y + player_height/2;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x + player_width;
                    bullet[i].y = player_y + player_height/2;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
        }
        else if(firemode == 3) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x;
                    bullet[i].y = player_y + player_height/2;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x + player_width;
                    bullet[i].y = player_y + player_height/2;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullet[i].bullet_active) {
                    bullet[i].x = player_x + player_width / 2;
                    bullet[i].y = player_y + player_height;
                    bullet[i].bullet_active = true;
                    break;
                }
            }
        }
    }

    void fire_bullet_enemy() {
        if(1 || score >= 200) {
            for (int i = 0; i < MAX_ENEMY; i++) {
                for(int j = 0; j < MAX_BULLETS_ENEMY; j++) {
                    if (!enemy[i].enemybullet[j].bullet_active) {
                        enemy[i].enemybullet[j].x = enemy[i].x + enemy[i].width / 2;
                        enemy[i].enemybullet[j].y = enemy[i].y;
                        enemy[i].enemybullet[j].bullet_active = true;
                        break;
                    }
                }
            }
        }
    }        

    void fire_missile_boss() {
        for(int i = 0; i < MAX_BOSS; i++) {
            if(!boss[i].missile.missile_active) {
                boss[i].missile.x = boss[i].x + (boss[i].width/2) - (boss[i].missile.width/2);
                boss[i].missile.y = boss[i].y - boss[i].missile.height;
                boss[i].missile.missile_active = true;
            
            }
        }
    }

    
    // Extra lives
    void live_spawn(int x, int y){
        for(int i = 0; i<MAX_LIVE; i++){
            if(!live[i].active){
                live[i].x = x;
                live[i].y = y;
                live[i].active = true;
            }
        }
    }

    void shield_spawn(int x, int y){
        for(int i = 0; i<MAX_SHIELD; i++){
            if(!shield[i].active){
                shield[i].x = x;
                shield[i].y = y;
                shield[i].active = true;
            }
        }
    }
    void firerate_spawn(int x, int y){

        if(!firerate.active){
            firerate.x = x;
            firerate.y = y;
            firerate.active = true;
        }
    }
    void shieldcounter(){
        shield_counter++;
        if(shield_counter % 200 == 0) {
            player_shield = false;
            shield_counter = 0;
        }
    }

    void saveinfo(char *name, int score) {
        FILE *fP = fopen("leaderboard.txt","a");
        fprintf(fP, "%s %d\n", name, score);
        fclose(fP);
    }

    void showinfo() {
        char finalscore[6];
        sprintf(finalscore, "%d",score);
        iSetColor(0, 0, 0);
        iText(390, 195, namebuffer, GLUT_BITMAP_TIMES_ROMAN_24);
        iText(410, 55, finalscore, GLUT_BITMAP_TIMES_ROMAN_24);
    }

    // Collision detection between bullets and asteroid
    void check_collision_bullet_asteroid() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                for (int j = 0; j < MAX_AST; j++) {
                    if (check_collision(bullet[i].x, bullet[i].y, 0, 0,
                        asteroid[j].x, asteroid[j].y, asteroid[j].width, asteroid[j].height) && asteroid[j].y < screen_height) {
                        bullet[i].bullet_active = false;
                        score += 10; 
                        if(rand() % 10 == 0 && health<=50){
                            live_spawn(asteroid[j].x, asteroid[j].y);
                        }
                        else if(rand() % 10 == 0 && health<=50){
                            shield_spawn(asteroid[j].x, asteroid[j].y);
                        }
                        else if(rand() % 3 && rand() % 1000 && score>=500 && firemode<3){
                            firerate_spawn(asteroid[j].x, asteroid[j].y);
                        }
                        explode.x = asteroid[j].x;
                        explode.y = asteroid[j].y;
                        explode.exp = true;
                        asteroid[j].x = rand() % (screen_width - asteroid[i].width);
                        asteroid[j].y = screen_height + rand() % 300;
                    }
                }
            }
        }
    }

    // Collision detection between bullets and missile
    void check_collision_bullet_missile() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                for (int j = 0; j < MAX_AST; j++) {
                    if (check_collision(bullet[i].x, bullet[i].y, 0, 0,
                        boss[j].missile.x, boss[j].missile.y, boss[j].missile.width, boss[j].missile.height)) {
                        bullet[i].bullet_active = false; 
                        score += 10; 
                        explode.x = boss[j].missile.x;
                        explode.y = boss[j].missile.y;
                        explode.exp = true;
                        boss[j].missile.missile_active = false;
                    }
                }
            }
        }
    }

    // Collision detection between bullets and boulder
    void check_collision_bullet_boulder() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                for (int j = 0; j < MAX_BOUL; j++) {
                    if (bullet[i].x > boulder[j].x &&
                        bullet[i].x < boulder[j].x + boulder[j].width &&
                        bullet[i].y > boulder[j].y &&
                        bullet[i].y < boulder[j].y + boulder[j].height && boulder[i].y < screen_height) {
                        bullet[i].bullet_active = false; 
                        score += 10; 
                        explode.x = boulder[j].x;
                        explode.y = boulder[j].y;
                        explode.exp = true;
                        boulder[j].x = rand() % (screen_width - boulder[j].width);
                        boulder[j].y = screen_height + rand() % 300;
                    }
                }
            }
        }
    }

    // Collision detection between bullets and enemy
    void check_collision_bullet_enemy() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                for (int j = 0; j < MAX_ENEMY; j++) {
                    if (check_collision(bullet[i].x, bullet[i].y, 0, 0,
                        enemy[j].x, enemy[j].y, enemy[j].width, enemy[j].height) && enemy[j].y < screen_height) {
                        bullet[i].bullet_active = false; 
                        explode.x = enemy[j].x;
                        explode.y = enemy[j].y;
                        explode.exp = true;
                        enemy[j].health -= 1;
                        
                        if(enemy[j].health == 0){
                            if(rand() % 10 == 0 && health<=50){
                            live_spawn(enemy[j].x, enemy[j].y);
                            }
                            else if(rand() % 7 == 0 && health<=50){
                                shield_spawn(enemy[j].x, enemy[j].y);
                            }
                            enemy[j].x = 100 + rand() % (screen_width - 200);
                            enemy[j].y = screen_height + rand() % 300;
                            enemy[j].health = 3;
                            score += 20; 
                        }
                    }
                }
            }
        }
    }

    // Collision detection between bullets and boss
    void check_collision_bullet_boss() {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                for(int j = 0; j < MAX_BOSS; j++) {    
                    if (check_collision(bullet[i].x, bullet[i].y, 0, 0,
                        boss[j].x, boss[j].y, boss[j].width, boss[j].height) && boss[j].y < screen_height) {
                        bullet[i].bullet_active = false;
                        explode.x = boss[j].x;
                        explode.y = boss[j].y;
                        explode.exp = true;
                        boss[j].health -= 1;
                        
                        if(boss[j].health == 0){
                            if(rand() % 7 == 0 && health<=50){
                            live_spawn(boss[j].x, boss[j].y);
                            }
                            else if(rand() % 7 == 0 && health<=50){
                                shield_spawn(boss[j].x, boss[j].y);
                            }
                            boss[j].x = 100 + rand() % (screen_width - 200);
                            boss[j].y = screen_height + rand() % 300;
                            boss[j].health = 1;
                            score += 20; 
                        }
                    }
                }
            }
        }
    }

    // Collision detection between player and asteroid,boulder,health item, shield item, enemy bullet, firerate, missile;
    void check_collision_player(){
        for(int i = 0;i < MAX_AST;i++){
            if(asteroid[i].x + asteroid[i].width > player_x &&
                asteroid[i].x < player_x + player_width &&
                asteroid[i].y + asteroid[i].height > player_y &&
                asteroid[i].y < player_y + player_height){
                    if(!player_shield)health -= 10;
                    explode.x = player_x;
                    explode.y = player_y;
                    explode.exp = true;
                    asteroid[i].x = rand() % (screen_width - asteroid[i].width);
                    asteroid[i].y = screen_height + rand() % 300;
                    }
                    
        }
        for(int i = 0; i<MAX_BOUL; i++)
        {
            if(boulder[i].x + boulder[i].width > player_x &&
                boulder[i].x < player_x + player_width &&
                boulder[i].y + boulder[i].height > player_y &&
                boulder[i].y < player_y + player_height){
                    boulder[i].x = rand() % (screen_width - boulder[i].width);
                    boulder[i].y = screen_height + rand() % 300;
                    if(!player_shield)health -= 5;
                    explode.x = player_x;
                    explode.y = player_y;
                    explode.exp = true;
                }
        }
        // health collision
        for(int i = 0; i<MAX_LIVE; i++)
        {
            if(check_collision(player_x, player_y, player_width, player_height,
                    live[i].x, live[i].y, live[i].width, live[i].height) && live[i].active){
                    health += 10;
                    live[i].active = false;
                    //break;
                }
        }
        // shield collision
        for(int i = 0; i < MAX_SHIELD; i++)
        {
            if(check_collision(player_x, player_y, player_width, player_height,
                    shield[i].x, shield[i].y, shield[i].width, shield[i].height) && shield[i].active){
                    shield[i].active = false;
                    player_shield = true;
                }
        }
        // firerate collision
        if(check_collision(player_x, player_y, player_width, player_height,
                    firerate.x, firerate.y, firerate.width, firerate.height) && firerate.active) {
                    if(firemode<3)firemode += 1;
                    firerate.active = false;
                }
    }
    
    void collision_player_bullet() {
        // enemy bullet collision
        for(int i = 0; i < MAX_ENEMY; i++) {
            for (int j = 0; j < MAX_BULLETS_ENEMY; j++) {
                if (enemy[i].enemybullet[j].bullet_active) {
                    if (enemy[i].enemybullet[j].x > player_x && enemy[i].enemybullet[j].x < player_x + player_width && 
                        enemy[i].enemybullet[j].y > player_y && enemy[i].enemybullet[j].y < player_y + player_height) {
                        if(!player_shield)health -= 10;
                        enemy[i].enemybullet[j].bullet_active = false; 
                        explode.x = player_x;
                        explode.y = player_y;
                        explode.exp = true;
                        break;
                    }
                }
            }
        }
    }

    void collision_player_missile() {
        // boss missile collision
        for(int i = 0; i < MAX_ENEMY; i++) {           
            if (boss[i].missile.missile_active) {
                if (boss[i].missile.x > player_x && boss[i].missile.x < player_x + player_width && 
                    boss[i].missile.y > player_y && boss[i].missile.y < player_y + player_height) {
                    if(!player_shield)health -= 30;
                    boss[i].missile.missile_active = false; // Deactivate missile
                    explode.x = player_x;
                    explode.y = player_y;
                    explode.exp = true;
                    break;
                }
            }
            
        }
    }

    void iMouseMove(int mx, int my) {
        //printf("x = %d, y= %d\n",mx,my);
        //place your codes here
    }

    void iPassiveMouseMove(int mx, int my){
        if(mx > 25 && mx < screen_width-25)player_x = mx - 25;
    }

    void iMouse(int button, int state, int mx, int my) {
        printf("x = %d, y= %d\n", mx, my);
        if (game_state == 0) { // Main Menu
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(385, 350, 30, 115, mx, my)) {
                game_state = 1; // Start Game
            } 
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(280, 310, 30, 320, mx, my)){
                game_state = 4; // Show leaderboard
            }
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(300, 270, 30, 280, mx, my)){
                game_state = 5; // Show instruction from main menu
            }
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(340, 230, 30, 195, mx, my)){
                game_state = 6; // Show settings/sounds
            }
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(330, 190, 30, 210, mx, my)){
                exit(0); // quit game
            }
        }
        if(game_state == 2) {
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(340, 310, 40, 200, mx, my)) {
                game_state = 1;
            } 
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(280, 250, 50, 330, mx, my)){
                game_state = 8; // Show instruction from pause menu
            }
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(325, 200, 40, 275, mx, my)){
                game_state = 7; // Show settings from pause menu
            }
            else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(310, 130, 50, 265, mx, my)){
                exit(0); // Show instruction
            }
        }
        if(game_state == 3) { // Game over
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(280, 180, 45, 300, mx, my)) {
                takename = 1;
            }
        }
        if(game_state == 4) {
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(295, 30, 55, 300, mx, my)) {
                game_state = 0;
            }
        }
        if(game_state == 5) {
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(295, 30, 50, 300, mx, my)) {
                game_state = 0;
            }
        }
        if(game_state == 6) { //settings from main menu
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(500, 250, 60, 80, mx, my)) {
                music = !music;
                if(music) {
                    PlaySound("Sounds\\mainbg.wav", NULL , SND_LOOP|SND_ASYNC);
                }
                else
                    PlaySound(0, 0, 0);
            }
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(295, 30, 50, 300, mx, my)) {
                game_state = 0;
            }
        }
        if(game_state == 7) { //settings from pause menu
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(500, 250, 60, 80, mx, my)) {
                music = !music;
                if(music) {
                    PlaySound("Sounds\\mainbg.wav", NULL , SND_LOOP|SND_ASYNC);
                }
                else
                    PlaySound(0, 0, 0);
            }
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(370, 700, 50, 140, mx, my)) {
                game_state = 2;
            }
        }
        if(game_state == 8) {
            if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && buttoncheck(370, 700, 50, 140, mx, my)) {
                game_state = 2;
            }
        }
    }

    void iKeyboard(unsigned char key) {
        if (game_state == 1) { // Gameplay
            if (key == 'p') {
            game_state = 2; // Pause Game
            }
            if(key == 'q'){
                exit(0);
            }
        }
        else if (game_state == 2) { // Pause Screen
            if (key == 'p') {
            game_state = 1; // Resume Game
            }
            if(key == 'q'){
                exit(0);
            }
        }
        else if(game_state == 3) { // Take name game over
            
            char *c = (char *)&key;
            if(takename) {
                if(key == '\r') {
                    saveinfo(namebuffer, score);
                    printf("%s %d\n", namebuffer, score);
                    for(int i = 0; i < 100;i++) {
                        namebuffer[i] = '\0';
                    }
                    printf("%s\n",namebuffer);
                    takename = 0;
                    gamereset();
                }
                else if(key == '\b') {
                    if(strlen(namebuffer)){
                        namebuffer[strlen(namebuffer)-1] = '\0';
                    }
                    else
                        namebuffer[0] = '\0';
                }
                else {
                    strcat(namebuffer, c);
                }
            }
            if(key == 'q'){
                exit(0);
            }
        }
        if (game_state == 1 && key == ' ') {
            fire_bullet(); // Fire bullet
        }
    }

    void iSpecialKeyboard(unsigned char key) {
        //place your codes for other keys here        
    }
    void draw_enemybullet() {
        iSetColor(0, 255, 0);
        for(int i = 0; i < MAX_ENEMY; i++) {
            for (int j = 0; j < MAX_BULLETS_ENEMY; j++) {
                if (enemy[i].enemybullet[j].bullet_active) {
                    //iFilledCircle(enemy[i].enemybullet[i].x, enemy[j].enemybullet[i].y, 5);
                    iShowBMP2(enemy[i].enemybullet[j].x, enemy[i].enemybullet[j].y, "BMP\\laser.bmp", 16777215);
                }
            }
        }
    }
    void draw_bullet() {
        iSetColor(255, 0, 0);
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullet[i].bullet_active) {
                //iFilledCircle(bullet[i].x, bullet[i].y, 5);
                iShowBMP2(bullet[i].x-2, bullet[i].y, "BMP\\laser2.bmp", 16777215);
            }
        }
    }
    void draw_explosion(){
        if(explode.exp && explode.x != 0 && explode.y != 0){
            iShowBMP2(explode.x, explode.y, "BMP\\collision.bmp", 16777215);
            iShowBMP2(explode.x, explode.y, "BMP\\collision2.bmp", 16777215);
            explode.exp = false;      
        }
    }
    void draw_main_menu() {
        iShowBMP(0, 0, "BMP\\meteor_rush_hd.bmp");
    }
    void draw_pause_screen() {
        iShowBMP(0, 0, "BMP\\pausebg.bmp");
    }
    void draw_gameover_screen(){
        iShowBMP(0, 0, "BMP\\gameoverbg.bmp");
    }
    void draw_settings_6() {
        if(music){
            iShowBMP(0, 0, "BMP\\settings_on_mainmenu.bmp");
        }
        else{
           iShowBMP(0, 0, "BMP\\settings_off_mainmenu.bmp"); 
        }
    }
    void draw_settings_7() {
        if(music){
            iShowBMP(0, 0, "BMP\\settings_on_pausemenu.bmp");
        }
        else{
           iShowBMP(0, 0, "BMP\\settings_off_pausemenu.bmp"); 
        }
    }
    void draw_instruction_5() {
        iShowBMP(0, 0, "BMP\\instruction-mainmenu.bmp");
    }    
    void draw_instruction_8() {
        iShowBMP(0, 0, "BMP\\instruction-pausemenu.bmp");
    }
    void draw_leaderboard() {
        char buffer_name[100];
        char buffer_score[10];
        iShowBMP(0, 0, "BMP\\leaderboard.bmp");
        iSetColor(0, 0, 0);
        for (int i = 0; i < MAX_LEADERBOARD; i++) {
            sprintf(buffer_name, "%d. %s", i + 1, leaderboard[i].name);
            sprintf(buffer_score, "%d", leaderboard[i].score);
            iText(240, 280 - (i * 30), buffer_name, GLUT_BITMAP_TIMES_ROMAN_24);
            iText(550, 280 - (i * 30), buffer_score, GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }
    void draw_enemy(){
        for(int i = 0; i < MAX_ENEMY; i++){
            //iFilledRectangle(enemy[i].x, enemy[i].y, enemy[i].width, enemy[i].height);
            iShowBMP2(enemy[i].x, enemy[i].y, "BMP\\enemy1.bmp", 0);            
        }
    }
    void draw_boss() {
        for(int i = 0; i < MAX_BOSS; i++) {
            //iFilledRectangle(boss[i].x, boss[i].y, 60, 60);
            iShowBMP2(boss[i].x, boss[i].y, "BMP\\boss.bmp", 16777215);
        }
    }
    void draw_missile() {
        for(int i = 0; i < MAX_BOSS; i++) {
            if(boss[i].missile.missile_active)iShowBMP2(boss[i].missile.x, boss[i].missile.y, "BMP\\missile.bmp", 16777215);
        }
    }
    void draw_live() {
        iSetColor(255, 0, 0);
            for(int i=0; i<MAX_LIVE; i++){
                if(live[i].active == true)iShowBMP2(live[i].x, live[i].y, "BMP\\OIP.bmp", 16777215);
            }
    }
    void draw_firerate() {
        if(firerate.active)iShowBMP2(firerate.x, firerate.y, "BMP\\fireratepowerup.bmp", 16777215);
    }
    void draw_shielditem() {
        iSetColor(255, 0, 0);
            for(int i=0; i<MAX_SHIELD; i++){
                if(shield[i].active == true)iShowBMP2(shield[i].x, shield[i].y, "BMP\\shield.bmp", 16777215);
            }
    }
    void draw_shield(){
        for(int i = 0; i<MAX_SHIELD; i++){
            if (player_shield){
                iSetColor(0, 255, 0);
                iCircle(player_x+player_width/2, player_y+player_height/2, 35, 100);
            }
        }
    }
    void draw_playerspaceship() {
        iShowBMP2(player_x,player_y,"BMP\\SpaceShipSprites.bmp", 16777215);
    }
    void draw_asteroid() {
        iSetColor(0, 0, 255);
            for (int i = 0; i < MAX_AST; i++) {
                //iFilledRectangle(asteroid[i].x, asteroid[i].y, enemy_width, enemy_height);
                iShowBMP2(asteroid[i].x, asteroid[i].y, "BMP\\Asteroids 01 (2).bmp", 0);
            }
    }
    void draw_boulder() {
        iSetColor(0, 255, 255);
            for (int i = 0; i < MAX_BOUL; i++) {
                //iFilledRectangle(boulder[i].x, boulder[i].y, boulder[i].width, boulder[i].height);
                iShowBMP2(boulder[i].x, boulder[i].y, "BMP\\Asteroids 01 (1).bmp", 16777215);
            }
    }
    void draw_background() {
        iShowBMP(0, 0, "BMP\\Blue_Nebula_01-512x512.bmp");
    }
    void draw_HUD() {
        
        int healthbarlen = ((float)health/100)*220;
        char score_text[20], health_text[20];
        sprintf(score_text, "%d", score);
        sprintf(health_text, "%d", health);
        iSetColor(0, 0, 0);
        iFilledRectangle(60,6, 226, 38);
        iSetColor(192, 192, 192);
        iFilledRectangle(63, 9, 220, 32);
        iSetColor(204, 102, 102);
        iFilledRectangle(63, 9, healthbarlen, 32);
        iSetColor(255, 0, 0);
        iText(21, 12, health_text);
        iSetColor(192, 192, 192);
        iFilledRectangle(687, 9, 140, 32);
        iSetColor(0, 0, 0);
        iText(720, 20, score_text, GLUT_BITMAP_HELVETICA_18);
        iSetColor(204, 102, 102);
        if(score<200) {
            iText(400, 20, "WAVE : 1", GLUT_BITMAP_HELVETICA_18);
        }
        else if(score < 800) {
            iText(400, 20, "WAVE : 2", GLUT_BITMAP_HELVETICA_18);
        }
        else 
            iText(400, 20, "WAVE : 3", GLUT_BITMAP_HELVETICA_18);
    }

    void iDraw() {
        iClear();

        if(game_state == 0){
            draw_main_menu();
        }
        if(game_state == 1){ 
            draw_background();// Draw background 

            draw_bullet();// Draw bullets
            
            draw_enemybullet();// Draw enemy bullets
           
            draw_missile(); // Draw missle enemy
            
            draw_live();  // Draw extra live item
            
            draw_firerate();// Draw firerate item
            
            draw_shielditem();// Draw shield item
            
            draw_shield();// Draw shield
            
            draw_playerspaceship();// Draw player spaceship
            
            draw_asteroid();// Draw asteroid
            
            draw_boulder();// Draw boulder
            
            draw_enemy(); // Draw enemy spaceship
            
            if(score >= 30)draw_boss(); // Draw boss
            
            draw_explosion(); // Draw explosion
            
            draw_HUD();// Display score and health 
        }
        if(game_state == 2){
            draw_pause_screen();
        }
        if(game_state == 3) { // Game Over
            draw_gameover_screen();
            showinfo();
        }
        if(game_state == 4) {
            draw_leaderboard();
        }
        if(game_state == 5) {
            draw_instruction_5();
        }
        if(game_state == 6) {
            draw_settings_6();
        }
        if(game_state == 7) {
            draw_settings_7();
        }
        if(game_state == 8) {
            draw_instruction_8();
        }
    }

    void update_game() {
        if(game_state == 1) {
            update_asteroid();
            update_boulder();
            update_dust();
            check_collision_bullet_asteroid();          
            if(score >= 800){
                update_boss();
                update_missile();   
                check_collision_bullet_missile();
                check_collision_bullet_boss();
                collision_player_missile();
            }
            if(score>=200){
                update_enemy();
                update_bullet_enemy();
                collision_player_bullet();
                check_collision_bullet_enemy();
            }
            check_collision_bullet_boulder();
            check_collision_player();
            update_live_spawn();
            update_shield_spawn();
            update_firerate_spawn();
            if(player_shield)shieldcounter();            
                        
            if (health <= 0) {
                game_state = 3;
            }
        }
    }

    int main() {
        srand(time(NULL));
        initialize_game();
        iSetTimer(20, update_game); // Update game state every 20ms
        iSetTimer(9000, shieldcounter); 
        iSetTimer(1000, fire_bullet_enemy); 
        iSetTimer(10, fire_missile_boss); 
        iSetTimer(1, update_bullets);  
        if(music){
            PlaySound("Sounds\\mainbg.wav", NULL , SND_LOOP|SND_ASYNC);
        }
        iInitialize(screen_width, screen_height, "Space Shooter");
        return 0;
    }