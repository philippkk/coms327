#ifndef CHARACTER_H
#define CHARACTER_H
class player{
    public:
        char symbol;
        int posX,posY;
        double accX,accY;
        double subX,subY;
        player(char s);
};
class enemy{
    public:
        char symbol;
        int posX,posY;
        enemy(char s);
};
class projectile{
    public:
        char symbol;
        int posX,posY;
        projectile(char s);
};
#endif