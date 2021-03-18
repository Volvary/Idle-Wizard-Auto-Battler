#pragma once

enum PlayStatus {	//Reason why a parchment should or should not be played.
	UnknownReason,

	Charging,
	NotEnoughBuff,
	NotEnoughDamage,
	CantCantrip,
	VoidManaFull,
	OtherBuffCharging,

	HandFull,
	BuffsReady,
	Cantripping,
	AlwaysPlay,
	ChargeBigSpell,
	Frenzy
};