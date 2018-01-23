#pragma once

typedef void* (*CreateInterfaceFn)(const char *Name, int *ReturnCode);

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)   
#define IN_SPEED					(1 << 17)	
#define IN_WALK						(1 << 18)	
#define IN_ZOOM						(1 << 19)	
#define IN_WEAPON1					(1 << 20)	
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)	
#define	IN_ATTACK3					(1 << 25)
#define	FL_ONGROUND					(1 << 0)	
#define FL_DUCKING					(1 << 1)	
#define	FL_WATERJUMP				(1 << 2)	
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)	
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)	
#define FL_FAKECLIENT				(1 << 8)	
#define	FL_INWATER					(1 << 9)										
#define	FL_FLY						(1 << 10)	
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)	
#define	FL_PARTIALGROUND			(1 << 17)	
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19) 
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)	
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)	
#define FL_OBJECT					(1 << 25) 
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)	
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30) 
#define PLAYER_FLAG_BITS			10
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

enum ECstrike15UserMessages
{
	CS_UM_VGUIMenu = 1,
	CS_UM_Geiger = 2,
	CS_UM_Train = 3,
	CS_UM_HudText = 4,
	CS_UM_SayText = 5,
	CS_UM_SayText2 = 6,
	CS_UM_TextMsg = 7,
	CS_UM_HudMsg = 8,
	CS_UM_ResetHud = 9,
	CS_UM_GameTitle = 10,
	CS_UM_Shake = 12,
	CS_UM_Fade = 13,
	CS_UM_Rumble = 14,
	CS_UM_CloseCaption = 15,
	CS_UM_CloseCaptionDirect = 16,
	CS_UM_SendAudio = 17,
	CS_UM_RawAudio = 18,
	CS_UM_VoiceMask = 19,
	CS_UM_RequestState = 20,
	CS_UM_Damage = 21,
	CS_UM_RadioText = 22,
	CS_UM_HintText = 23,
	CS_UM_KeyHintText = 24,
	CS_UM_ProcessSpottedEntityUpdate = 25,
	CS_UM_ReloadEffect = 26,
	CS_UM_AdjustMoney = 27,
	CS_UM_UpdateTeamMoney = 28,
	CS_UM_StopSpectatorMode = 29,
	CS_UM_KillCam = 30,
	CS_UM_DesiredTimescale = 31,
	CS_UM_CurrentTimescale = 32,
	CS_UM_AchievementEvent = 33,
	CS_UM_MatchEndConditions = 34,
	CS_UM_DisconnectToLobby = 35,
	CS_UM_PlayerStatsUpdate = 36,
	CS_UM_DisplayInventory = 37,
	CS_UM_WarmupHasEnded = 38,
	CS_UM_ClientInfo = 39,
	CS_UM_XRankGet = 40,
	CS_UM_XRankUpd = 41,
	CS_UM_CallVoteFailed = 45,
	CS_UM_VoteStart = 46,
	CS_UM_VotePass = 47,
	CS_UM_VoteFailed = 48,
	CS_UM_VoteSetup = 49,
	CS_UM_ServerRankRevealAll = 50,
	CS_UM_SendLastKillerDamageToClient = 51,
	CS_UM_ServerRankUpdate = 52,
	CS_UM_ItemPickup = 53,
	CS_UM_ShowMenu = 54,
	CS_UM_BarTime = 55,
	CS_UM_AmmoDenied = 56,
	CS_UM_MarkAchievement = 57,
	CS_UM_MatchStatsUpdate = 58,
	CS_UM_ItemDrop = 59,
	CS_UM_GlowPropTurnOff = 60,
	CS_UM_SendPlayerItemDrops = 61,
	CS_UM_RoundBackupFilenames = 62,
	CS_UM_SendPlayerItemFound = 63,
	CS_UM_ReportHit = 64,
	CS_UM_XpUpdate = 65,
	CS_UM_QuestProgress = 66,
	CS_UM_ScoreLeaderboardData = 67,
	CS_UM_PlayerDecalDigitalSignature = 68,
	MAX_ECSTRIKE15USERMESSAGES
};

enum ObserverModes
{
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM = 1,
	OBS_MODE_FREEZECAM = 2,
	OBS_MODE_FIXED = 3,
	OBS_MODE_IN_EYE = 4,
	OBS_MODE_CHASE = 5,
	OBS_MODE_ROAMING = 6
};

enum ItemDefinitionIndex
{
	weapon_none,
	weapon_deagle,
	weapon_elite,
	weapon_fiveseven,
	weapon_glock,
	weapon_ak47 = 7,
	weapon_aug,
	weapon_awp,
	weapon_famas,
	weapon_g3sg1,
	weapon_galilar = 13,
	weapon_m249,
	weapon_m4a4 = 16,
	weapon_mac10,
	weapon_p90 = 19,
	weapon_ump45 = 24,
	weapon_xm1014,
	weapon_bizon,
	weapon_mag7,
	weapon_negev,
	weapon_sawedoff,
	weapon_tec9,
	weapon_taser,
	weapon_p2000,
	weapon_mp7,
	weapon_mp9,
	weapon_nova,
	weapon_p250,
	weapon_scar20 = 38,
	weapon_sg556,
	weapon_ssg08,
	weapon_knife_ct = 42,
	weapon_flashbang,
	weapon_hegrenade,
	weapon_smokegrenade,
	weapon_molotov,
	weapon_decoy,
	weapon_incgrenade,
	weapon_c4,
	weapon_knife_t = 59,
	weapon_m4a1s = 60,
	weapon_usp = 61,
	weapon_cz75 = 63,
	weapon_revolver,
	weapon_bayonet = 500,
	weapon_flip = 505,
	weapon_gut,
	weapon_karambit,
	weapon_m9bayonet,
	weapon_huntsman,
	weapon_falchion = 512,
	weapon_bowie = 514,
	weapon_butterfly = 515,
	weapon_pushdagger,
	studded_bloodhound_gloves = 5027,
	sporty_gloves = 5030,
	slick_gloves = 5031,
	leather_handwraps = 5032,
	motorcycle_gloves = 5033,
	specialist_gloves = 5034
};

enum ClassId
{
	ClassId_CTestTraceline = 196,
	ClassId_CTEWorldDecal = 197,
	ClassId_CTESpriteSpray = 194,
	ClassId_CTESprite = 193,
	ClassId_CTESparks = 192,
	ClassId_CTESmoke = 191,
	ClassId_CTEShowLine = 189,
	ClassId_CTEProjectedDecal = 186,
	ClassId_CFEPlayerDecal = 61,
	ClassId_CTEPlayerDecal = 185,
	ClassId_CTEPhysicsProp = 182,
	ClassId_CTEParticleSystem = 181,
	ClassId_CTEMuzzleFlash = 180,
	ClassId_CTELargeFunnel = 178,
	ClassId_CTEKillPlayerAttachments = 177,
	ClassId_CTEImpact = 176,
	ClassId_CTEGlowSprite = 175,
	ClassId_CTEShatterSurface = 188,
	ClassId_CTEFootprintDecal = 172,
	ClassId_CTEFizz = 171,
	ClassId_CTEExplosion = 169,
	ClassId_CTEEnergySplash = 168,
	ClassId_CTEEffectDispatch = 167,
	ClassId_CTEDynamicLight = 166,
	ClassId_CTEDecal = 164,
	ClassId_CTEClientProjectile = 163,
	ClassId_CTEBubbleTrail = 162,
	ClassId_CTEBubbles = 161,
	ClassId_CTEBSPDecal = 160,
	ClassId_CTEBreakModel = 159,
	ClassId_CTEBloodStream = 158,
	ClassId_CTEBloodSprite = 157,
	ClassId_CTEBeamSpline = 156,
	ClassId_CTEBeamRingPoint = 155,
	ClassId_CTEBeamRing = 154,
	ClassId_CTEBeamPoints = 153,
	ClassId_CTEBeamLaser = 152,
	ClassId_CTEBeamFollow = 151,
	ClassId_CTEBeamEnts = 150,
	ClassId_CTEBeamEntPoint = 149,
	ClassId_CTEBaseBeam = 148,
	ClassId_CTEArmorRicochet = 147,
	ClassId_CTEMetalSparks = 179,
	ClassId_CSteamJet = 142,
	ClassId_CSmokeStack = 135,
	ClassId_DustTrail = 246,
	ClassId_CFireTrail = 64,
	ClassId_SporeTrail = 252,
	ClassId_SporeExplosion = 251,
	ClassId_RocketTrail = 249,
	ClassId_SmokeTrail = 250,
	ClassId_CPropVehicleDriveable = 122,
	ClassId_ParticleSmokeGrenade = 248,
	ClassId_CParticleFire = 100,
	ClassId_MovieExplosion = 247,
	ClassId_CTEGaussExplosion = 174,
	ClassId_CEnvQuadraticBeam = 56,
	ClassId_CEmbers = 46,
	ClassId_CEnvWind = 60,
	ClassId_CPrecipitation = 116,
	ClassId_CPrecipitationBlocker = 117,
	ClassId_CBaseTempEntity = 18,
	ClassId_NextBotCombatCharacter = 0,
	ClassId_CEconWearable = 45,
	ClassId_CBaseAttributableItem = 4,
	ClassId_CEconEntity = 44,
	ClassId_CWeaponXM1014 = 244,
	ClassId_CWeaponTaser = 239,
	ClassId_CSmokeGrenade = 133,
	ClassId_CWeaponSG552 = 236,
	ClassId_CSensorGrenade = 129,
	ClassId_CWeaponSawedoff = 232,
	ClassId_CWeaponNOVA = 228,
	ClassId_CIncendiaryGrenade = 87,
	ClassId_CMolotovGrenade = 97,
	ClassId_CWeaponM3 = 220,
	ClassId_CKnifeGG = 94,
	ClassId_CKnife = 93,
	ClassId_CHEGrenade = 84,
	ClassId_CFlashbang = 66,
	ClassId_CWeaponElite = 211,
	ClassId_CDecoyGrenade = 40,
	ClassId_CDEagle = 39,
	ClassId_CWeaponUSP = 243,
	ClassId_CWeaponM249 = 219,
	ClassId_CWeaponUMP45 = 242,
	ClassId_CWeaponTMP = 241,
	ClassId_CWeaponTec9 = 240,
	ClassId_CWeaponSSG08 = 238,
	ClassId_CWeaponSG556 = 237,
	ClassId_CWeaponSG550 = 235,
	ClassId_CWeaponScout = 234,
	ClassId_CWeaponSCAR20 = 233,
	ClassId_CSCAR17 = 127,
	ClassId_CWeaponP90 = 231,
	ClassId_CWeaponP250 = 230,
	ClassId_CWeaponP228 = 229,
	ClassId_CWeaponNegev = 227,
	ClassId_CWeaponMP9 = 226,
	ClassId_CWeaponMP7 = 225,
	ClassId_CWeaponMP5Navy = 224,
	ClassId_CWeaponMag7 = 223,
	ClassId_CWeaponMAC10 = 222,
	ClassId_CWeaponM4A1 = 221,
	ClassId_CWeaponHKP2000 = 218,
	ClassId_CWeaponGlock = 217,
	ClassId_CWeaponGalilAR = 216,
	ClassId_CWeaponGalil = 215,
	ClassId_CWeaponG3SG1 = 214,
	ClassId_CWeaponFiveSeven = 213,
	ClassId_CWeaponFamas = 212,
	ClassId_CWeaponBizon = 207,
	ClassId_CWeaponAWP = 205,
	ClassId_CWeaponAug = 204,
	ClassId_CAK47 = 1,
	ClassId_CWeaponCSBaseGun = 209,
	ClassId_CWeaponCSBase = 208,
	ClassId_CC4 = 29,
	ClassId_CWeaponBaseItem = 206,
	ClassId_CBaseCSGrenade = 8,
	ClassId_CSmokeGrenadeProjectile = 134,
	ClassId_CSensorGrenadeProjectile = 130,
	ClassId_CMolotovProjectile = 98,
	ClassId_CItem_Healthshot = 91,
	ClassId_CItemDogtags = 92,
	ClassId_CDecoyProjectile = 41,
	ClassId_CFireCrackerBlast = 62,
	ClassId_CInferno = 88,
	ClassId_CChicken = 31,
	ClassId_CFootstepControl = 68,
	ClassId_CCSGameRulesProxy = 34,
	ClassId_CWeaponCubemap = 0,
	ClassId_CWeaponCycler = 210,
	ClassId_CTEPlantBomb = 183,
	ClassId_CTEFireBullets = 170,
	ClassId_CTERadioIcon = 187,
	ClassId_CPlantedC4 = 108,
	ClassId_CCSTeam = 38,
	ClassId_CCSPlayerResource = 36,
	ClassId_CCSPlayer = 35,
	ClassId_CCSRagdoll = 37,
	ClassId_CTEPlayerAnimEvent = 184,
	ClassId_CHostage = 85,
	ClassId_CHostageCarriableProp = 86,
	ClassId_CBaseCSGrenadeProjectile = 9,
	ClassId_CHandleTest = 83,
	ClassId_CTeamplayRoundBasedRulesProxy = 146,
	ClassId_CSpriteTrail = 140,
	ClassId_CSpriteOriented = 139,
	ClassId_CSprite = 138,
	ClassId_CRagdollPropAttached = 125,
	ClassId_CRagdollProp = 124,
	ClassId_CPredictedViewModel = 118,
	ClassId_CPoseController = 114,
	ClassId_CGameRulesProxy = 82,
	ClassId_CInfoLadderDismount = 89,
	ClassId_CFuncLadder = 74,
	ClassId_CTEFoundryHelpers = 173,
	ClassId_CEnvDetailController = 52,
	ClassId_CWorld = 245,
	ClassId_CWaterLODControl = 203,
	ClassId_CWaterBullet = 202,
	ClassId_CVoteController = 201,
	ClassId_CVGuiScreen = 200,
	ClassId_CPropJeep = 121,
	ClassId_CPropVehicleChoreoGeneric = 0,
	ClassId_CTriggerSoundOperator = 199,
	ClassId_CBaseVPhysicsTrigger = 22,
	ClassId_CTriggerPlayerMovement = 198,
	ClassId_CBaseTrigger = 20,
	ClassId_CTest_ProxyToggle_Networkable = 195,
	ClassId_CTesla = 190,
	ClassId_CBaseTeamObjectiveResource = 17,
	ClassId_CTeam = 145,
	ClassId_CSunlightShadowControl = 144,
	ClassId_CSun = 143,
	ClassId_CParticlePerformanceMonitor = 101,
	ClassId_CSpotlightEnd = 137,
	ClassId_CSpatialEntity = 136,
	ClassId_CSlideshowDisplay = 132,
	ClassId_CShadowControl = 131,
	ClassId_CSceneEntity = 128,
	ClassId_CRopeKeyframe = 126,
	ClassId_CRagdollManager = 123,
	ClassId_CPhysicsPropMultiplayer = 106,
	ClassId_CPhysBoxMultiplayer = 104,
	ClassId_CPropDoorRotating = 120,
	ClassId_CBasePropDoor = 16,
	ClassId_CDynamicProp = 43,
	ClassId_CProp_Hallucination = 119,
	ClassId_CPostProcessController = 115,
	ClassId_CPointWorldText = 113,
	ClassId_CPointCommentaryNode = 112,
	ClassId_CPointCamera = 111,
	ClassId_CPlayerResource = 110,
	ClassId_CPlasma = 109,
	ClassId_CPhysMagnet = 107,
	ClassId_CPhysicsProp = 105,
	ClassId_CStatueProp = 141,
	ClassId_CPhysBox = 103,
	ClassId_CParticleSystem = 102,
	ClassId_CMovieDisplay = 99,
	ClassId_CMaterialModifyControl = 96,
	ClassId_CLightGlow = 95,
	ClassId_CItemAssaultSuitUseable = 0,
	ClassId_CItem = 0,
	ClassId_CInfoOverlayAccessor = 90,
	ClassId_CFuncTrackTrain = 81,
	ClassId_CFuncSmokeVolume = 80,
	ClassId_CFuncRotating = 79,
	ClassId_CFuncReflectiveGlass = 78,
	ClassId_CFuncOccluder = 77,
	ClassId_CFuncMoveLinear = 76,
	ClassId_CFuncMonitor = 75,
	ClassId_CFunc_LOD = 70,
	ClassId_CTEDust = 165,
	ClassId_CFunc_Dust = 69,
	ClassId_CFuncConveyor = 73,
	ClassId_CFuncBrush = 72,
	ClassId_CBreakableSurface = 28,
	ClassId_CFuncAreaPortalWindow = 71,
	ClassId_CFish = 65,
	ClassId_CFireSmoke = 63,
	ClassId_CEnvTonemapController = 59,
	ClassId_CEnvScreenEffect = 57,
	ClassId_CEnvScreenOverlay = 58,
	ClassId_CEnvProjectedTexture = 55,
	ClassId_CEnvParticleScript = 54,
	ClassId_CFogController = 67,
	ClassId_CEnvDOFController = 53,
	ClassId_CCascadeLight = 30,
	ClassId_CEnvAmbientLight = 51,
	ClassId_CEntityParticleTrail = 50,
	ClassId_CEntityFreezing = 49,
	ClassId_CEntityFlame = 48,
	ClassId_CEntityDissolve = 47,
	ClassId_CDynamicLight = 42,
	ClassId_CColorCorrectionVolume = 33,
	ClassId_CColorCorrection = 32,
	ClassId_CBreakableProp = 27,
	ClassId_CBeamSpotlight = 25,
	ClassId_CBaseButton = 5,
	ClassId_CBaseToggle = 19,
	ClassId_CBasePlayer = 15,
	ClassId_CBaseFlex = 12,
	ClassId_CBaseEntity = 11,
	ClassId_CBaseDoor = 10,
	ClassId_CBaseCombatCharacter = 6,
	ClassId_CBaseAnimatingOverlay = 3,
	ClassId_CBoneFollower = 26,
	ClassId_CBaseAnimating = 2,
	ClassId_CAI_BaseNPC = 0,
	ClassId_CBeam = 24,
	ClassId_CBaseViewModel = 21,
	ClassId_CBaseParticleEntity = 14,
	ClassId_CBaseGrenade = 13,
	ClassId_CBaseCombatWeapon = 7,
	ClassId_CBaseWeaponWorldModel = 23,
};

enum ECSPlayerBones
{
	pelvis = 2,
	spine_0,
	spine_1,
	spine_2,
	spine_3,
	neck_0,
	head_0,
	clavicle_L,
	arm_upper_L,
	arm_lower_L,
	hand_L,
	hand_R
};

enum LifeState : unsigned char
{
	LIFE_ALIVE = 0,// alive
	LIFE_DYING = 1, // playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD = 2, // dead. lying still.
	MAX_LIFESTATE
};

enum WeaponSound_t
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE, // Can't be "DOUBLE" because windows.h uses it.
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	FAST_RELOAD,
	// Add new shoot sound types here
	REVERSE_THE_NEW_SOUND,
	NUM_SHOOT_SOUND_TYPES,
	MAX_WEAPONSOUND
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4,
	MAX_MOVETYPE
};