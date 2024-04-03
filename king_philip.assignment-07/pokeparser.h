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
class pokemon_species{
    public:
        int id,generation_id,evolves_from_species_id,evolution_chain_id,color_id,shape_id
        ,habitat_id,gender_rate,capture_rate,base_happiness,is_baby,hatch_counter
        ,has_gender_differences,growth_rate_id,forms_switchable,is_legendary,is_mythical
        ,order,conquest_order;
        std::string indentifier;
        pokemon_species(int iD,std::string i,int genid,int efsid,int evcid,int cid,int sid,int hid,int gr,int cr,int bh,int ib, int hc
        ,int hgd,int grid,int fs,int il,int im,int o,int co);
        pokemon_species();
        void printPokemonSpecies();

};
class experience{
    public:
        int growth_rate_id,level,exp;
        experience(int grid,int l,int e);
        experience();
        void printExperience();
};
class type_names{
    public:
        int type_id,local_language_id;
        std::string name;
        type_names(int tid,int llid, std::string n);
        type_names();
        void printTypeNames();
};
class pokemon_stats{
    public:
        int pokemon_id,stat_id,base_stat,effort;
        pokemon_stats(int pid,int sid,int bs,int e);
        pokemon_stats();
        void printPokemonStats();
};
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
    std::vector<pokemon_species> parsePokemonSpecies();
    std::vector<experience> parseExperience();
    std::vector<type_names> parseTypeNames();
    std::vector<pokemon_stats> parsePokemonStats();
    std::string pathOne;
    std::string pathTwo;
    std::string pathThree;

};



#endif