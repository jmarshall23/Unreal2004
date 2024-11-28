//=============================================================================
// Alien Female Voice.
//=============================================================================
class AlienFemaleVoice extends xVoicePack;

#exec OBJ LOAD FILE=TauntPack.uax

defaultproperties
{
	NameSound(0)=TauntPack.fn_redleader
	NameSound(1)=TauntPack.fn_blueleader

	////////////////// ACKNOWELDGEMENTS
	AckSound(0)=TauntPack.fn_affirmative
	AckSound(1)=TauntPack.fn_got_it
	AckSound(2)=TauntPack.fn_im_on_it
	AckSound(3)=TauntPack.fn_roger

	////////////////// FRIENDLY FIRE
	FFireSound(0)=TauntPack.fn_im_on_your_team
	FFireSound(1)=TauntPack.fn_im_on_your_team_idiot
	FFireSound(2)=TauntPack.fn_same_team

	////////////////// TAUNTS
	numTaunts=28

	// GENERIC
	TauntSound(0)=TauntPack.fn_and_stay_down
	TauntSound(1)=TauntPack.fn_anyoneelsewantsome
	TauntSound(2)=TauntPack.fn_boom
	TauntSound(3)=TauntPack.fn_burnbaby
	TauntSound(4)=TauntPack.fn_diebitch
	TauntSound(5)=TauntPack.fn_eatthat
	TauntSound(6)=TauntPack.fn_fightlikenali
	TauntSound(7)=TauntPack.fn_isthatyourbest
	TauntSound(8)=TauntPack.fn_kissmyass
	TauntSound(9)=TauntPack.fn_loser
	TauntSound(10)=TauntPack.fn_myhouse
	TauntSound(11)=TauntPack.fn_next
	TauntSound(12)=TauntPack.fn_ohyeah
	TauntSound(13)=TauntPack.fn_ownage
	TauntSound(14)=TauntPack.fn_seeya
	TauntSound(15)=TauntPack.fn_that_had_to_hurt
	TauntSound(16)=TauntPack.fn_useless
	TauntSound(17)=TauntPack.fn_you_play_like_a_girl
	TauntSound(18)=TauntPack.fn_youbedead
	TauntSound(19)=TauntPack.fn_youlikethat
	TauntSound(20)=TauntPack.fn_youwhore

	TauntSound(21)=TauntPack.fn_blowme
	TauntSound(22)=TauntPack.fn_camper
	TauntSound(23)=TauntPack.fn_douchebag
	TauntSound(24)=TauntPack.fn_spammer
	TauntSound(25)=TauntPack.fn_sucker
	TauntSound(26)=TauntPack.fn_talktothehand
	TauntSound(27)=TauntPack.fn_youdick

	TauntString(21)="Blow me"
	TauntString(22)="Camper!"
	TauntString(23)="Douche bag!"
	TauntString(24)="Spammer!"
	TauntString(25)="Sucker"
	TauntString(26)="Talk to the hand"
	TauntString(27)="You dick!"

	MatureTaunt(21)=1
	MatureTaunt(23)=1
	MatureTaunt(27)=1

	HumanOnlyTaunt(22)=1
	HumanOnlyTaunt(24)=1
	HumanOnlyTaunt(26)=1

	////////////////// DEATH
	NumDeathPhrases=4

	// GENERIC
	DeathPhrases(0)=TauntPack.fn_medic
	DeathPhrases(1)=TauntPack.fn_nice

	// CUSTOM

	////////////////// ORDERS

	OrderSound(0)=TauntPack.fn_defend_the_base
	OrderSound(1)=TauntPack.fn_HoldThisPosition
	OrderSound(2)=TauntPack.fn_attack_their_base
	OrderSound(3)=TauntPack.fn_cover_me
	OrderSound(4)=TauntPack.fn_SearchAndDestroy
	OrderSound(10)=TauntPack.fn_Take_Their_Flag
	OrderSound(11)=TauntPack.fn_Defend_The_Flag
	OrderSound(12)=TauntPack.fn_AttackAlpha
	OrderSound(13)=TauntPack.fn_AttackBravo
	OrderSound(14)=TauntPack.fn_GetTheBall

	////////////////// OTHERS

	OtherSound(0)=TauntPack.fn_base_is_undefended
	OtherSound(1)=TauntPack.fn_get_our_flag_back
	OtherSound(2)=TauntPack.fn_igottheflag
	OtherSound(3)=TauntPack.fn_ive_got_your_back
	OtherSound(4)=TauntPack.fn_im_hit
	OtherSound(5)=TauntPack.fn_man_down
	OtherSound(6)=TauntPack.fn_im_all_alone_here
	OtherSound(7)=TauntPack.fn_Negative
	OtherSound(8)=TauntPack.fn_got_it
	OtherSound(9)=TauntPack.fn_in_position
	OtherSound(10)=TauntPack.fn_im_going_in
	OtherSound(11)=TauntPack.fn_area_secure
	OtherSound(12)=TauntPack.fn_enemy_flag_carrier_is_here
	OtherSound(13)=TauntPack.fn_i_need_backup
	OtherSound(14)=TauntPack.fn_incoming
	OtherSound(15)=TauntPack.fn_ball_carrier_is_here
	OtherSound(16)=TauntPack.fn_point_alpha_secure
	OtherSound(17)=TauntPack.fn_point_bravo_secure
	OtherSound(18)=TauntPack.fn_attackalpha
	OtherSound(19)=TauntPack.fn_attackbravo
	OtherSound(20)=TauntPack.fn_the_base_is_under_attack
	OtherSound(21)=TauntPack.fn_were_being_overrun
	OtherSound(22)=TauntPack.fn_under_heavy_attack
	OtherSound(23)=TauntPack.fn_defend_point_alpha
	OtherSound(24)=TauntPack.fn_defend_point_bravo
	OtherSound(25)=TauntPack.fn_gettheball
	OtherSound(26)=TauntPack.fn_im_on_defense
	OtherSound(27)=TauntPack.fn_im_on_offense
	OtherSound(28)=TauntPack.fn_take_point_alpha
	OtherSound(29)=TauntPack.fn_take_point_bravo
	OtherSound(30)=TauntPack.fn_medic
	OtherSound(31)=TauntPack.fn_nice

	VoicePackName="Female Alien"
	VoiceGender=VG_Female
}
