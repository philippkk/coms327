#ifndef POKEPARSER_H
#define POKEPARSER_H
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
class pokemon{
    private:
        
    public:
        int id, species_id,height,weight,base_experience,order,is_default;
        std::string identifier;
        pokemon();
        pokemon(int iD, std::string i, int sid,int h, int w, int be, int o, int isd);
        void printPokemon();
};
class moves{
    public:
        int id,generation_id,type_id,power,pp,accuracy,
            priority,target_id,damage_class_id,
            effect_id,effect_chance,contest_type_id,
            contest_effect_id,super_contest_effect_id;
        std::string indentifier;
        moves(int iD, std::string i,int gen,int type,int pwr,int p, int a , 
            int pri,int tid,int dci,
            int eid,int ec,int cti, int ceid,int sceid);
        moves();
        void printMoves();
};
class pokemon_moves{
    public:
        int pokemon_id,version_group_id,move_id,pokemon_move_method_id,level,order;
        pokemon_moves(int pid,int vgid,int mid,int pmmid,int l,int o);
        pokemon_moves();
        void printPokemonMoves();

};
class pokemon_species{};
class experience{};
class type_names{};
class pokemon_stats{};
class stats{};
class pokemon_types{};

class pokeparser
{
private:
    
public:
    pokeparser();
    ~pokeparser();
    std::vector<pokemon> parsePokemon();
    std::vector<moves> parseMoves();
    std::vector<pokemon_moves> parsePokemonMoves();
    std::string pathOne;
    std::string pathTwo;
    std::string pathThree;

};



#endif