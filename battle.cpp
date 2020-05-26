#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <map>

#include "battle.hpp"
#include "creature.hpp"

BattleEvent::BattleEvent(Creature* source, Creature* target, BattleEventType type)
{
	this->source = source;
	this->target = target;
	this->type = type;
}

int BattleEvent::run()
{
	switch(type)
	{
		case BattleEventType::ATTACK:
			return source->attack(target);
		case BattleEventType::ATTACK_FAST:
			return source->attack_fast(target);
		case BattleEventType::ATTACK_STRONG:
			return source->attack(target);
		case BattleEventType::DEFEND:
			return source->defend();
		case BattleEventType::FOCUS:
			return source->focus();
		case BattleEventType::DEFEND_ENEMY:
			return source->defend_enemy(target);
		case BattleEventType::FOCUS_ENEMY:
			return source->focus_enemy(target);
		default:
			return 0;
	}
}

void Battle::kill(Creature* creature)
{
	// Find the creature's position in the combatants vector
	auto pos = std::find(this->combatants.begin(), this->combatants.end(), creature);

	// Don't try and delete the creature if it doesn't exist
	if(pos != this->combatants.end())
	{
		std::cout << creature->id << " is slain!\n"; // uwaga, zmiana id moze zmienic kolejnosc działań

		// Health == 0 is used in main as a condition to check if the creature is
		// dead, but this function could be called when the creature is not killed
		// by reducing their health to zero (by a death spell, for example), so we
		// ensure the creature's health is 0 and is marked as dead
		creature->hp = 0;
		// Remove the creature
		this->combatants.erase(pos);
	}

	return;
}
	//przydzielanie unikalnego id dla kilku przeciwników?

void Battle::run()
{
	std::vector<Creature*>::iterator player;
	std::vector<Creature*>::iterator end;

	//backup postaci i przeciwnika
	
	do
	{
		// walcz, aż zostanie jeden 
		player = std::find_if(this->combatants.begin(), this->combatants.end(),
			[](Creature* a) { return a->id == "player"; });
		end = this->combatants.end();

		this->nextTurn();
	}
	while(player != end && this->combatants.size() > 1);

	return;
}

void Battle::nextTurn()
{
	// kolejka, gracz pierwszy ("player">"enemy")
	std::queue<BattleEvent> events;
	std::sort(combatants.begin(), combatants.end(), [](Creature* a, Creature* b) { return a->id > b->id; });//jak gracz to na początek
	// Iterate over the combatants and decide what they should do,
	// before adding the action to the event queue.
	for(auto com : this->combatants)
	{
		int enemy_action = rand() % 5 + 1;
		switch(enemy_action)
		{
			case 1:
			{
				std::cout<<"Przeciwnik szykuje unik!\n";
				break;
			}
			case 2:
			{
				std::cout<<"Przeciwnik bedzie sie bronil!\n";
				break;
			}
			default:
			{
				std::cout<<"Przeciwnik zaatakuje!\n";
				break;
			}
		}
		
		if(com->id == "player")
		{
			//wybór przeciwnika
			/*
			// Create the target selection dialogue
			Dialogue targetSelection = Dialogue("Who?", {});
			// Created every turn because some combatants may die
			for(auto target : this->combatants)
			{
				if(target->id != "player")
				{
					targetSelection.addChoice(target->id);
				}
			}
			*/
			
			// zapytaj o akcję
			std::cout<<"Co chcesz zrobic?\n1. Atak\n2. Szybki atak\n3. Silny atak\n4. Obrona\n5. Skupienie (na uniku)\n";
			int choice;
			std::cin>>choice;
			//takie małe obejście dla obrony i uniku przeciwnika
			if (enemy_action == 1)
			{
				int position = 1;
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::DEFEND_ENEMY));
			}
			if (enemy_action == 2)
			{
				int position = 1;
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::FOCUS_ENEMY));
			}
			
			switch(choice)
			{
				default:
				case 1:
				{
					// Player is attacking, so ask for the target.
					// Dialogue returns the number of the choice but with
					// the player removed, so we have to do some fancy
					// arithmetic to find the actual location of the target
					// and then convert that to a pointer
					
					// dla kilku przeciwników
					//int position = targetSelection.activate();
					int position = 1;
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::ATTACK));
					break;
				}
				case 2:
				{
					int position = 1;
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::ATTACK_FAST));
					break;
				}
				case 3:
				{
					int position = 1;
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::ATTACK_STRONG));
					break;
				}
				case 4:
				{
					// Player is defending
					events.push(BattleEvent(com, nullptr, BattleEventType::DEFEND));
					break;
				}
				case 5:
				{
					//Player dodges
					events.push(BattleEvent(com,nullptr, BattleEventType::FOCUS));
					break;
				}
			}
		}
		else
		{
			if (enemy_action != 1 || enemy_action != 2)
			{
				// Simple enemy AI where enemy constantly attacks player
				Creature* player = *std::find_if(this->combatants.begin(), this->combatants.end(),
					[](Creature* a) { return a->id == "player"; });
				events.push(BattleEvent(com, player, BattleEventType::ATTACK));
			}
		}
	}

	// Take each event from the queue in turn and process them,
	// displaying the results
	while(!events.empty())
	{
		// Take event from the front of the queue
		BattleEvent event = events.front();
		switch(event.type)
		{
			case BattleEventType::ATTACK:
			{
				// The event can't be run if either the source or the
				// target were slain previously in this turn, so we
				// must check that they're valid first
				auto a = this->combatants.begin();
				auto b = this->combatants.end();
				if(std::find(a, b, event.source) == b || std::find(a, b, event.target) == b)
				{
					break;
				}
				//działa? 
				std::cout << event.source->id
					<< " attacks "
					<< event.target->id
					<< " for "
					<< event.run()
					<< " damage!\n";
				// Delete slain enemies
				if(event.target->hp <= 0)
				{
					this->kill(event.target);
				}
				break;
			}
			case BattleEventType::ATTACK_FAST:
			{
				auto a = this->combatants.begin();
				auto b = this->combatants.end();
				if(std::find(a, b, event.source) == b || std::find(a, b, event.target) == b)
				{
					break;
				}
				//działa? 
				std::cout << event.source->id
					<< " attacks "
					<< event.target->id
					<< " for "
					<< event.run()
					<< " damage!\n";
				// Delete slain enemies
				if(event.target->hp <= 0)
				{
					this->kill(event.target);
				}
				break;
			}
			case BattleEventType::ATTACK_STRONG:
			{
				auto a = this->combatants.begin();
				auto b = this->combatants.end();
				if(std::find(a, b, event.source) == b || std::find(a, b, event.target) == b)
				{
					break;
				}
				//działa? 
				std::cout << event.source->id
					<< " attacks "
					<< event.target->id
					<< " for "
					<< event.run()
					<< " damage!\n";
				// Delete slain enemies
				if(event.target->hp <= 0)
				{
					this->kill(event.target);
				}
				break;
			}
			case BattleEventType::DEFEND:
			{
				std::cout << "Defense up!\n";
				std::cout << event.source->defense
					<<" -> "
					<< event.run()
					<< "!\n";
				break;
			}
			case BattleEventType::FOCUS:
			{
				std::cout << "Agility up!\n";
				std::cout << event.source->agility
					<<" -> "
					<< event.run()
					<< "!\n";
				break;
			}
			case BattleEventType::DEFEND_ENEMY:
			{
				std::cout << "Enemy agility up!\n";
				std::cout << event.target->defense
					<<" -> "
					<< event.run()
					<< "!\n";
				break;
			}
			case BattleEventType::FOCUS_ENEMY:
			{
				std::cout << "Enemy agility up!\n";
				std::cout << event.target->agility
					<<" -> "
					<< event.run()
					<< "!\n";
				break;
			}
			default:
				break;
		}
		events.pop();
	}
}