//###########################################################################################################################################################################################################################################
// Copyright (C) Juin 2020 Stitch pour Aquayoup
// AI generique npc par classe : DRUIDE V1.4
// Il est possible d'influencer le temp entre 2 cast avec `BaseAttackTime` & `RangeAttackTime` 
// Necessite dans Creature_Template :
// Minimun  : UPDATE `creature_template` SET `ScriptName` = 'Stitch_npc_ai_druide',`AIName` = '' WHERE (entry = 15100001);
// Optionel : UPDATE `creature_template` SET `HealthModifier` = 2, `ManaModifier` = 3, `ArmorModifier` = 1, `DamageModifier` = 2,`BaseAttackTime` = 2000, `RangeAttackTime` = 2000 WHERE(entry = 15100001);
// Optionel : Utilisez pickpocketloot de creature_template pour passer certains parametres (Solution choisit afin de rester compatible avec tout les cores). Si pickpocketloot = 1 (branche1 forc�), pickpocketloot = 2 (branche2 forc�), etc
//###########################################################################################################################################################################################################################################
// # npc de Test Stitch_npc_ai_druide  .npc 15100001
// REPLACE INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
// (15100001, 0, 0, 0, 0, 0, 55745, 0, 0, 0, 'Stitch_npc_ai_Druide', '', '', '', 0, 90, 90, 0, 0, 2102, 0, 1.01, 1.01, 0.8, 0, 0, 2000, 2000, 1, 1, 2, 0, 2048, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 2, 1, 3, 1, 1.5, 2, 1, 0, 144, 1, 0, 0, 'Stitch_npc_ai_druide', 20173);
// REPLACE INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES
// (15100001, 1, 69208, 0, 0, 0);
//###########################################################################################################################################################################################################################################





//################################################################################################
//StitchAI AI Druide
//################################################################################################

class Stitch_npc_ai_druide : public CreatureScript
{
public: Stitch_npc_ai_druide() : CreatureScript("Stitch_npc_ai_druide") { }

		struct Stitch_npc_ai_druideAI : public ScriptedAI
		{
			Stitch_npc_ai_druideAI(Creature* creature) : ScriptedAI(creature) { }

			uint32 BrancheSpe = 1;													// Choix de la Sp�cialisation : Equilibre=1, Ours=2, Felin=3, Tr�ant=4
			uint32 NbrDeSpe = 4;													// Nombre de Sp�cialisations
			uint32 DistanceDeCast = 30;												// Distance max a laquelle un npc attaquera , au dela il quite le combat
			uint32 ResteADistance = 15;												// Distance max a laquelle un npc s'approchera
			uint32 Dist;															// Distance entre le npc et sa cible
			uint32 Random;
			uint32 Mana;
			uint32 MaxMana = me->GetMaxPower(POWER_MANA);

			uint32 Modelid_Branche2 = 2281;											// Modelid Ours 2281 , Gros Ours 1990
			uint32 Modelid_Branche3 = 892;											// Modelid f�lin 892
			uint32 Modelid_Branche4 = 57040;										// Modelid Petite-Dranche 57040

			Unit* victim = me->GetVictim();											// La cible du npc
			uint32 Buf_all = 1126;													// Marque du fauve 1126
			uint32 Buf_branche1 = 22812;											// Ecorce = 22812
			uint32 Buf_branche2 = 15727;											// Rugissement d�moralisant 15727
			uint32 Buf_branche3 = 33907;											// Epines 33907, Hurlement furieux 3149 (force +15, Rouge)
			uint32 Buf_branche4 = 33907;											// Epines 33907

			// Spells Divers
			uint32 Drain_de_vie = 689;												// Drain de vie 
			uint32 Charge_ours = 32323;												// Charge
			uint32 BOND_Aleatoire = 70485;											// BOND Aleatoire
			uint32 Empaler_et_tirer = 82742;										// Empaler et tirer
			uint32 Barbeles_depines = 113967;										// Barbel�s d'�pines 
			uint32 Lenteur_Treant = 6146;											// Lenteur

			// Spells Equilibre
			uint32 Spell_branche1_agro;
			uint32 Spell_branche1_1;
			uint32 Spell_branche1_2;
			uint32 Spell_branche1_3;
			uint32 branche1_agro[4] = { 2912, 2912, 2912, 33844 };					// Feu stellaire 2912, Sarments (4s) 33844
			uint32 branche1_1[2] = { 119577, 5176 };								// Col�re (verte) 119577 , Col�re (jaune) 5176
			uint32 branche1_2[2] = { 78674, 770 };									// �ruption stellaire 78674, Lucioles 770
			uint32 branche1_3[2] = { 15798, 2912 };									// Eclat lunaire 15798, Feu stellaire 2912
																					
			// Definitions des Spells Ours
			uint32 Spell_branche2_agro;
			uint32 Spell_branche2_1;
			uint32 Spell_branche2_2;
			uint32 Spell_branche2_3;
			uint32 branche2_agro[3] = { 32323, 22863, 19196 };						// Charge 32323, Vitesse 22863 (30%) , charge afflux 19196 (charge+renverse)
			uint32 branche2_1[2] = { 85835, 84867 };								// Mutiler 85835/300206, Balayage fracassant 84867
			uint32 branche2_2[2] = { 33917, 106832 };								// Mutilation (mob ours) 33917/300207, Rosser Ours 106832/300208
			uint32 branche2_3[5] = { 131172, 300209, 138766, 10887, 127987 };		// Rugissement per�ant 138766 (interrompt ,emp�chant les sorts de la m�me �cole pendant 6 sec)
																					// Coup de tete 131172, Coup de cr�ne 300209 (dps+interrompt  4s), faucher la foule 10887 (interomp, repousse 10 m), *Morsure h�morragique 127987
			// Spells Felin
			uint32 Spell_branche3_agro;
			uint32 Spell_branche3_1;
			uint32 Spell_branche3_2;
			uint32 Spell_branche3_3;
			uint32 branche3_agro[3] = { 22863, 89712, 59737 };						// Vitesse +30% 22863, Griffure bondissante 89712 (Etourdi 2s),vitesse +100% 59737
			uint32 branche3_1[2] = { 24187, 85835 };								// Griffe 24187, Mutiler 85835
			uint32 branche3_2[2] = { 113687, 138765 };								// Morsure 113687, �craser 138765 (armure -10%/30s)
			uint32 branche3_3[4] = { 300220, 300220, 83639, 30639 };				// Griffure 300220, Bain de sang 83639 (6 attaque /3s), Morsure carnivore 30639

			// Spells Tr�ant 
			uint32 Spell_branche4_agro;
			uint32 Spell_branche4_1;
			uint32 Spell_branche4_2;
			uint32 Spell_branche4_3;
			uint32 branche4_agro[3] = { 33844, 82742, 82742 };						// Sarments 4s 33844, *Empaler et tirer 82742
			uint32 branche4_1[3] = { 131193, 119004, 38742 };						// Choc 131193, Violent coup direct 119004, Encha�nement gangren� 38742
			uint32 branche4_2[4] = { 80515, 113967, 113967, 118682 };				// Fr�n�sie immortelle 80515 (Canalis�e 5s), Barbel�s d'�pines 113967, Taper du pied 118682
			uint32 branche4_3[4] = { 129375, 689, 152571, 152571 };					// Choc terrestre 129375 (stun 3s), Drain de vie 689, Encorner 152571 (cone 5m)

			uint32 Spell_Heal = 5185;  												// Toucher gu�risseur 5185

		// Definitions des variables Cooldown et le 1er lancement
			uint32 Cooldown_Spell1 = 500;
			uint32 Cooldown_Spell2 = 2500;
			uint32 Cooldown_Spell3 = 3500;
			uint32 Cooldown_Spell_Heal = 3000;
			uint32 Cooldown_RegenMana = 3000;
			uint32 Cooldown_ResteADistance = 1000;									// Test si en contact pour Equilibre pour s'eloigner, bouger en combat pour le Felin
			uint32 Cooldown_Charge = 8000;

			void JustRespawned() override
				{
					// Pour visuel seulement
					me->setPowerType(POWER_MANA);
					me->SetMaxPower(POWER_MANA, MaxMana);
					me->SetPower(POWER_MANA, MaxMana);
					me->RestoreDisplayId();												// Retire Morph
					me->GetMotionMaster()->MoveTargetedHome();							// Retour home

					me->SetSpeedRate(MOVE_RUN, 1.01f);
				}
			void EnterCombat(Unit* /*who*/) override
			{
				if (!UpdateVictim())
					return;
			
				Unit* victim = me->GetVictim();

				// Forcer le choix de la Sp�cialisation par creature_template->pickpocketloot
				uint32 ForceBranche = me->GetCreatureTemplate()->pickpocketLootId;					// creature_template->pickpocketloot
				if (ForceBranche == 1) { BrancheSpe = 1; }											// branche1 forc�
				else if (ForceBranche == 2) { BrancheSpe = 2; }										// branche2 forc�
				else if (ForceBranche == 3) { BrancheSpe = 3; }										// branche3 forc�
				else if (ForceBranche == 4) { BrancheSpe = 4; }										// branche4 forc�
				else
				{
				// Sinon Choix de la Sp�cialisation Al�atoire
				BrancheSpe = urand(1, NbrDeSpe + 2);
				if (BrancheSpe > NbrDeSpe) { BrancheSpe = 1; }								// plus de chance d'etre equilibre
				}

				me->SetReactState(REACT_AGGRESSIVE);


				// Spell a lancer a l'agro ---------------------------------------------------------------------------------------------------------------------
					me->CastSpell(me, Buf_all, true);										// Buf1 sur lui meme pour toutes les S�pialit�es

					switch(BrancheSpe)
					{
					case 1: // Si Sp�cialisation Equilibre -----------------------------------------------------------------------------------------------------
						me->SetSpeedRate(MOVE_RUN, 1.01f);

						// Pour visuel seulement
						VisuelMana();

						me->CastSpell(me, Buf_branche1, true);								// Buf2 sur lui meme

						// Tirages al�atoires des spells Equilibre 
						Spell_branche1_agro = branche1_agro[urand(0, 3)];
						Spell_branche1_1 = branche1_1[urand(0, 1)];
						Spell_branche1_2 = branche1_2[urand(0, 1)];
						Spell_branche1_3 = branche1_3[urand(0, 1)];

						AttackStartCaster(victim, ResteADistance);							// Distance de cast
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);				// ROOT

						Random = urand(1, 3); 
						if (Random == 1) { DoCastVictim(Spell_branche1_agro); }				// 1/3 Chance de lancer le sort d'agro
					break;

					case 2: // Si Sp�cialisation Ours  --------------------------------------------------------------------------------------------------------
						me->SetSpeedRate(MOVE_RUN, 1.1f);									// Vitesse de d�placement

						Bonus_Armure(200);													// Bonus d'armure +100%
						me->CastSpell(me, Buf_branche2, true);								// Buf3 sur lui meme

						// Pour visuel seulement
						VisuelRage();
						me->SetDisplayId(Modelid_Branche2);									// Modelid Ours

						// Tirages al�atoires des spells Ours 
						Spell_branche2_agro = branche2_agro[urand(0, 2)];
						Spell_branche2_1 = branche2_1[urand(0, 1)];
						Spell_branche2_2 = branche2_2[urand(0, 1)];
						Spell_branche2_3 = branche2_3[urand(0, 4)];

						Random = urand(1, 3);
						if (Random == 1) { DoCastVictim(Spell_branche2_agro); }				// 1/3 Chance de lancer le sort d'agro
					break;

					case 3: // Si Sp�cialisation Felin  --------------------------------------------------------------------------------------------------------
						me->SetSpeedRate(MOVE_RUN, 1.1f);									// Vitesse de d�placement

						Bonus_Armure(110);													// Bonus d'armure +10%
						me->CastSpell(me, Buf_branche3, true);								// Buf3 sur lui meme

						// Pour visuel seulement
						VisuelEnergy();
						me->SetDisplayId(Modelid_Branche3);									// Modelid Felin
																							// Tirages al�atoires des spells Equilibre 
						Spell_branche3_agro = branche3_agro[urand(0, 2)];
						Spell_branche3_1 = branche3_1[urand(0, 1)];
						Spell_branche3_2 = branche3_2[urand(0, 1)];
						Spell_branche3_3 = branche3_3[urand(0, 3)];

						Random = urand(1, 3);
						if (Random == 1) { DoCastVictim(Spell_branche3_agro); }				// 1/3 Chance de lancer le sort d'agro
						break;

					case 4: // Si Sp�cialisation Tr�ant ---------------------------------------------------------------------------------------------------------
						me->SetSpeedRate(MOVE_RUN, 0.9f);

						Bonus_Armure(150);													// Bonus d'armure +50%
						me->CastSpell(me, Buf_branche4, true);								// Buf4 sur lui meme

						// Pour visuel seulement
						me->SetDisplayId(Modelid_Branche4);									// Modelid Tr�ant - Petite-Branche 57040

						// Tirages al�atoires des spells Petite-Branche 
						Spell_branche4_agro = branche4_agro[urand(0, 2)];
						Spell_branche4_1 = branche4_1[urand(0, 2)];
						Spell_branche4_2 = branche4_2[urand(0, 3)];
						Spell_branche4_3 = branche4_3[urand(0, 3)];

						Random = urand(1, 2);
						if (Random == 1) { DoCastVictim(Spell_branche4_agro); }				// 1/2 Chance de lancer le sort d'agro
						break;

					}

			}
			void JustReachedHome() override
			{
				// Pour visuel seulement
				me->setPowerType(POWER_MANA);
				me->SetMaxPower(POWER_MANA, MaxMana);
				me->SetPower(POWER_MANA, MaxMana);

				me->DeMorph();
				me->SetSpeedRate(MOVE_RUN, 1.01f);								// Vitesse par defaut d�finit a 1.01f puisque le patch modification par type,famille test si 1.0f

				me->RemoveAura(Buf_all);
				me->RemoveAura(Buf_branche1);
				me->RemoveAura(Buf_branche2);
				me->RemoveAura(Buf_branche3);
				me->RemoveAura(Buf_branche4);

				Bonus_Armure(100);												// Retire bonus d'armure

				RetireBugDeCombat();
				me->SetReactState(REACT_AGGRESSIVE);
			}
			void EnterEvadeMode(EvadeReason /*why*/) override
			{
				RetireBugDeCombat();
				me->AddUnitState(UNIT_STATE_EVADE);
				me->SetSpeedRate(MOVE_RUN, 1.5f);											// Vitesse de d�placement
				me->GetMotionMaster()->MoveTargetedHome();									// Retour home
			}
			void UpdateAI(uint32 diff) override
			{
				if (!UpdateVictim()/* || me->isPossessed() || me->IsCharmed() || me->HasAuraType(SPELL_AURA_MOD_FEAR)*/)
					return;

				Mana = me->GetPower(POWER_MANA);
				Dist = me->GetDistance(me->GetVictim());


				// Combat suivant la Sp�cialisation
				switch (BrancheSpe)
				{
				case 1: // Sp�cialisation Equilibre ################################################################################################################

					Mouvement_Caster(diff);
					Mouvement_All();
					Combat_Equilibre(diff);
					break;

				case 2 : // Sp�cialisation Ours ####################################################################################################################

					Combat_Ours(diff);
					Mouvement_Ours(diff);
					Mouvement_All();
					break;

				case 3: // Sp�cialisation Felin ####################################################################################################################
			
					Combat_Felin(diff);
					Mouvement_Felin(diff);
					Mouvement_All();
					break;

				case 4: // Sp�cialisation Tr�ant ######################################################################################################################

					Combat_Treant(diff);
					Mouvement_Treant(diff);
					Mouvement_All();

					// heal sur lui meme
					if ( (me->GetHealth() < (me->GetMaxHealth()*0.6)) && (Cooldown_Spell_Heal <= diff)	)	// Si PV < 60%
					{
						DoCastVictim(Drain_de_vie);																	// Drain de vie 689
						Cooldown_Spell_Heal = 8000;
					}
					break;
				}
			}

			void RetireBugDeCombat()
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);				// UNROOT
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);					// Retire flag "en combat"
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);				// Retire flag "non attaquable"

				me->DeleteThreatList();
				me->CombatStop(true);
				me->LoadCreaturesAddon();
				me->SetLootRecipient(NULL);
				me->ResetPlayerDamageReq();
				me->SetLastDamagedTime(0);
			}
			void VisuelMana()
			{
				me->setPowerType(POWER_MANA);
				me->SetMaxPower(POWER_MANA, MaxMana);
				me->SetPower(POWER_MANA, MaxMana);
			}
			void VisuelRage()
			{
				me->SetMaxPower(POWER_MANA, 1000);
				me->setPowerType(POWER_RAGE);
				me->SetMaxPower(POWER_RAGE, 1000);
				me->SetPower(POWER_RAGE, 1000);
			}
			void VisuelEnergy()
			{
				me->SetMaxPower(POWER_MANA, 100);
				me->setPowerType(POWER_ENERGY);
				me->SetMaxPower(POWER_ENERGY, 100);
				me->SetPower(POWER_ENERGY, 100);
			}

			void Mouvement_All()
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());
				if ( (Dist > DistanceDeCast) | (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 40) )
				{
					RetireBugDeCombat();
					me->AddUnitState(UNIT_STATE_EVADE);
					EnterEvadeMode(EVADE_REASON_SEQUENCE_BREAK);						// Quite le combat si la cible > 30m (Caster & M�l�e) ou > 40m de home
				}
			}
			void Mouvement_Caster(uint32 diff)
			{
				if ( !UpdateVictim() )
					return;

				Mana = me->GetPower(POWER_MANA);
				Unit* victim = me->GetVictim();
				Dist = me->GetDistance(victim);

				// Mouvement al�atoire si cible < 6m & Mana > 5% --------------------------------------------------------------------------------------------
				if (Cooldown_ResteADistance <= diff)
				{
					if ((Dist < 6) && (Mana > MaxMana / 20))
					{
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);							// UNROOT
						me->SetSpeedRate(MOVE_RUN, 1.1f);

						float x, y, z;
						me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, ResteADistance + 5);			// Bouge de ResteADistance + 5m
						me->GetMotionMaster()->MovePoint(0xFFFFFE, x, y, z);

						Cooldown_ResteADistance = 5000;
					}
				}
				else Cooldown_ResteADistance -= diff;

				// Mouvement OFF si Mana > 5% & distance > 5m & <= 20m ---------------------------------------------------------------------------------------------
				if ((Mana > MaxMana / 20) && (Dist > 5) && (Dist <= ResteADistance) )
				{
					AttackStartCaster(me->GetVictim(), ResteADistance);						// Distance de cast

					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);					// ROOT
					me->SetSpeedRate(MOVE_RUN, 1.01f);
				}

				// Mouvement ON si distance > 20m -----------------------------------------------------------------------------------------------
				if (Dist > 20)
				{
					AttackStartCaster(me->GetVictim(), ResteADistance);						// Distance de cast

					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);				// UNROOT
					me->SetSpeedRate(MOVE_RUN, 1.01f);
				}
			
					// Mouvement ON si Mana < 5%  -----------------------------------------------------------------------------------------------
					if (Mana < MaxMana / 20)
					{
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);			// UNROOT
						me->SetSpeedRate(MOVE_RUN, 1.01f);
					} 

			}
			void Mouvement_Ours(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());

				// Mouvement Ours -----------------------------------------------------------------------------------------------------------------------------------
				if (Cooldown_Charge <= diff)
				{
					Random = urand(1, 2);
					if ((Dist >= 8) && (Dist <= 20))
					{
						if (Random = 1)
						{
							DoSpellAttackIfReady(Charge_ours);						// Charge 32323 - 1 chance sur 2
						}
						Cooldown_Charge = 15000;
					}
				} else Cooldown_Charge -= diff;
			}
			void Mouvement_Felin(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());

				// Mouvement Felin -----------------------------------------------------------------------------------------------------------------------------------

				// Si la cible est entre 8 & 30m : Griffure bondissante
				if (Cooldown_Charge <= diff)
				{
					Random = urand(1, 4);
					if ((Dist >= 8) && (Dist <= DistanceDeCast))
					{
						if (Random = 1)
						{
							DoSpellAttackIfReady(89712);									// Griffure bondissante - 1 chance sur 4
						}
						Cooldown_Charge = urand(15000,20000);
						Cooldown_ResteADistance = urand(2000, 4000);						// Sinon bugue BOND Aleatoire ou Avance !?
					}
				}
				else Cooldown_Charge -= diff;

				// Si la cible < 8m : BOND Aleatoire ou Avance
				if (Dist < 8 && (Cooldown_ResteADistance <= diff))
				{
					Random = urand(1, 5);
					if (Random == 1)
					{
						DoCast(me, BOND_Aleatoire);													// 1 chance sur 5 BOND Aleatoire 70485 Au contact
					}
					else
					{
						 // Au contact tourne au tour de sa victime --------------------------------------------------------------------------------------------------
						 Unit* victim = me->GetVictim();
						 Dist = me->GetDistance(me->GetVictim());

						 float x, y, z;
						 x = (victim->GetPositionX() + urand(0, 4) - 2);
						 y = (victim->GetPositionY() + urand(0, 4) - 2);
						 z = victim->GetPositionZ();
						 me->GetMotionMaster()->MovePoint(0, x, y, z);						// 4 chance sur 5 avance Au contact
					}
					Cooldown_ResteADistance = urand(2000,4000);
				}
				else Cooldown_ResteADistance -= diff;
			}
			void Mouvement_Treant(uint32 diff)
			{
				// Mouvement Tr�ant  ----------------------------------------------------------------------------------------------------------------------------------
				if (Cooldown_Charge <= diff)
				{
					Random = urand(1, 5);
					if ((Dist >= 8) && (Dist <= 20))
					{
						if (Random = 1)
						{
							DoSpellAttackIfReady(Empaler_et_tirer);											// Empaler et tirer - 1 chance sur 2
						} else
							DoSpellAttackIfReady(Barbeles_depines);											// Barbel�s d'�pines 113967

						Cooldown_Charge = 5000;
					}
				}
				else Cooldown_Charge -= diff;
			}

			void Combat_Equilibre(uint32 diff)
			{
				if (!UpdateVictim() /*| (me->HasUnitState(UNIT_STATE_MOVING))*/ )
					return;

				Mana = me->GetPower(POWER_MANA);
				Dist = me->GetDistance(me->GetVictim());

				// Combat - Equilibre --------------------------------------------------------------------------------------------------------------------------
				if (Dist <= ResteADistance)
				{
					// Regen mana en combat ------------------------------------------------------------------------------------------------------------------------
					if (Cooldown_RegenMana <= diff)
					{
						me->SetPower(POWER_MANA, Mana + (MaxMana / 20));
						if (Mana > MaxMana) { me->SetPower(POWER_MANA, MaxMana); }
						Cooldown_RegenMana = 1000;
					}
					else Cooldown_RegenMana -= diff;

					Heal_En_Combat_Caster(diff);

					// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff)
					{
						DoCastVictim(Spell_branche1_1);
						Cooldown_Spell1 = 3500;
					}
					else Cooldown_Spell1 -= diff;

					// Spell2 sur la cible chaque (Sort secondaire plus lent)
					if (Cooldown_Spell2 <= diff)
					{
						DoCastVictim(Spell_branche1_2);
						Cooldown_Spell2 = urand(4000, 6000);
					}
					else Cooldown_Spell2 -= diff;

					// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
					if (Cooldown_Spell3 <= diff)
					{
						DoCastVictim(Spell_branche1_3);
						Cooldown_Spell3 = urand(8000, 10000);
					}
					else Cooldown_Spell3 -= diff;

				}
			}
			void Combat_Ours(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());
				// Combat Ours --------------------------------------------------------------------------------------------------------------------------------
				if (Dist < 6)
				{
					// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff)
					{
						//DoCastVictim(Spell_branche2_1);
						DoCastAOE(Spell_branche2_1, true);
						Cooldown_Spell1 = 3000;
					}
					else Cooldown_Spell1 -= diff;

					// Spell2 sur la cible chaque (Sort secondaire plus lent)
					if (Cooldown_Spell2 <= diff)
					{
						//DoCastVictim(Spell_branche2_2);
						DoCastAOE(Spell_branche2_2, true);
						Cooldown_Spell2 = 4000;
					}
					else Cooldown_Spell2 -= diff;

					// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
					if (Cooldown_Spell3 <= diff)
					{
						//DoCastVictim(Spell_branche2_3);
						DoCastAOE(Spell_branche2_3, true);
						Cooldown_Spell3 = 8000;
					}
					else Cooldown_Spell3 -= diff;

				}
			}
			void Combat_Felin(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());

				// Combat Felin --------------------------------------------------------------------------------------------------------------------------------
				if (Dist < 6)
				{
					// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff)
					{
						//DoCastVictim(Spell_branche3_1);
						DoCastAOE(Spell_branche3_1, true);
						Cooldown_Spell1 = 2750;
					}
					else Cooldown_Spell1 -= diff;

					// Spell2 sur la cible chaque (Sort secondaire plus lent)
					if (Cooldown_Spell2 <= diff)
					{
						//DoCastVictim(Spell_branche3_2);
						DoCastAOE(Spell_branche3_2, true);
						Cooldown_Spell2 = 4000;
					}
					else Cooldown_Spell2 -= diff;

					// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
					if (Cooldown_Spell3 <= diff)
					{
						//DoCastVictim(Spell_branche3_3);
						DoCastAOE(Spell_branche3_3, true);
						Cooldown_Spell3 = urand(10000, 14000);
					}
					else Cooldown_Spell3 -= diff;

				}


			}
			void Combat_Treant(uint32 diff)
			{
				if (!UpdateVictim())
					return;

				Dist = me->GetDistance(me->GetVictim());

				// Combat Treant -------------------------------------------------------------------------------------------------------------------------
				if ((Dist < 6) && (!me->HasUnitState(UNIT_STATE_CASTING)))
				{
					// Spell1 sur la cible chaque (Sort R�guli�)
					if (Cooldown_Spell1 <= diff)
					{
						DoCastVictim(Spell_branche4_1);
						Cooldown_Spell1 = 3000;
					}
					else Cooldown_Spell1 -= diff;

					// Spell2 sur la cible chaque (Sort secondaire plus lent)
					if (Cooldown_Spell2 <= diff)
					{
						DoCastVictim(Spell_branche4_2);
						me->CastSpell(victim, Lenteur_Treant, true);										//Lenteur 6146 sur la cible
						me->CastSpell(me, Lenteur_Treant, true);											//Lenteur 6146 sur lui
						Cooldown_Spell2 = 5000;
					}
					else Cooldown_Spell2 -= diff;

					// Spell3 sur la cible  (Sort secondaire tres lent , g�n�ralement utilis� comme Dot)
					if (Cooldown_Spell3 <= diff)
					{
						DoCastVictim(Spell_branche4_3);
						Cooldown_Spell3 = 10000;
					}
					else Cooldown_Spell3 -= diff;
				}
			}

			void Heal_En_Combat_Caster(uint32 diff)
			{
			// Heal en combat ------------------------------------------------------------------------------------------------------------------------------
			if (Cooldown_Spell_Heal <= diff)
			{
				Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, DistanceDeCast, true);		// pour heal friend

				// heal sur lui meme
				if ((me->GetHealth() < (me->GetMaxHealth()*0.75)))								// Si PV < 75%
				{
					DoCast(me, Spell_Heal);
					Cooldown_Spell_Heal = 2000;
				}

				// heal sur Friend 
				if (target = DoSelectLowestHpFriendly(DistanceDeCast))							// Distance de l'alli� = 30m
				{
					if (me->IsFriendlyTo(target) && (me != target))
					{
						if (target->GetHealth() < (target->GetMaxHealth()*0.60))				// Si PV < 60%
						{
							DoCast(target, Spell_Heal);
							Cooldown_Spell_Heal = 2000;
						}
					}
				}
			}
			else Cooldown_Spell_Heal -= diff;
			}

			void Bonus_Armure(int val) // 
			{
				// +- Bonus d'armure 100% = pas de bonus/malus
				me->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, me->GetArmor() * (val / 100)); // 0
				me->SetCanModifyStats(true);
				me->UpdateAllStats();
			}
			void Bonus_Degat_Arme_Done(int val) // 
			{
				// +- Bonus en % de degat des armes inflig�es a victim
				me->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, val, true);
				me->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, val, true);
				me->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_PCT, val, true);
			}

		};



		CreatureAI* GetAI(Creature* creature) const override
		{
			return new Stitch_npc_ai_druideAI(creature);
		}
};

//################################################################################################
void AddSC_npc_ai_druide()
{
	new Stitch_npc_ai_druide();
}















