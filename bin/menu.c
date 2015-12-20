/*
 * menu.c
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "war2edit.h"


/*============================================================================*
 *                                 Private API                                *
 *============================================================================*/

static Evas_Object *
_radio_add(Editor          *ed,
           Evas_Object     *group,
           unsigned int     object,
           Elm_Object_Item *parent,
           const char      *label,
           Evas_Smart_Cb    func)
{
   Evas_Object *o;
   Elm_Object_Item *eoi;

   o = elm_radio_add(ed->menu);
   eo_do(o, elm_obj_radio_state_value_set(object));
   if (group != NULL)
     eo_do(o, elm_obj_radio_group_add(group));

   if (label) elm_object_text_set(o, label);
   if (parent)
     {
        eoi = elm_menu_item_add(ed->menu, parent, NULL, NULL, func, o);
        elm_object_item_content_set(eoi, o);
     }

   return o;
}

static void
_radio_changed_common_do(Evas_Object *radio,
                         int         *bind)
{
   int val;

   val = elm_radio_state_value_get(radio);
   elm_radio_value_set(radio, val);
   *bind = val;
}


/*============================================================================*
 *                                  Callbacks                                 *
 *============================================================================*/

static void
_win_del_cb(void        *data,
            Evas_Object *obj   EINA_UNUSED,
            void        *event EINA_UNUSED)
{
   Editor *ed = data;
   editor_free(ed);
}

static void
_win_new_cb(void        *data  EINA_UNUSED,
            Evas_Object *obj   EINA_UNUSED,
            void        *event EINA_UNUSED)
{
}

static void
_win_open_cb(void        *data,
             Evas_Object *obj   EINA_UNUSED,
             void        *event EINA_UNUSED)
{
   Editor *ed = data;
   file_load_prompt(ed);
}

static void
_win_save_cb(void        *data,
             Evas_Object *obj   EINA_UNUSED,
             void        *event EINA_UNUSED)
{
   Editor *ed = data;
   if (!ed->pud->filename)
     file_save_prompt(ed);
   else
     editor_save(ed, ed->pud->filename);
}

static void
_win_save_as_cb(void        *data,
                Evas_Object *obj   EINA_UNUSED,
                void        *event EINA_UNUSED)
{
   Editor *ed = data;
   file_save_prompt(ed);
}

static void
_map_properties_cb(void        *data,
                   Evas_Object *obj   EINA_UNUSED,
                   void        *event EINA_UNUSED)
{
   Editor *ed = data;

   if (inwin_id_is(ed, INWIN_MAP_PROPERTIES))
     inwin_activate(ed);
   else
     {
        inwin_set(ed, menu_map_properties_new(ed, ed->inwin.obj),
                  INWIN_MAP_PROPERTIES,
                  "Close", NULL, NULL, NULL);
     }
}

static void
_player_properties_cb(void        *data  EINA_UNUSED,
                      Evas_Object *obj   EINA_UNUSED,
                      void        *event EINA_UNUSED)
{
}

static void
_starting_properties_cb(void        *data  EINA_UNUSED,
                        Evas_Object *obj   EINA_UNUSED,
                        void        *event EINA_UNUSED)
{
}

static void
_units_properties_cb(void        *data  EINA_UNUSED,
                     Evas_Object *obj   EINA_UNUSED,
                     void        *event EINA_UNUSED)
{
}

static void
_upgrades_properties_cb(void        *data  EINA_UNUSED,
                        Evas_Object *obj   EINA_UNUSED,
                        void        *event EINA_UNUSED)
{
}

static void
_radio_units_changed_cb(void        *data,
                        Evas_Object *obj,
                        void        *event EINA_UNUSED)
{
   unsigned int w, h;
   Editor *ed = evas_object_data_get(obj, "editor");
   _radio_changed_common_do(data, (int *)(&(ed->sel_unit)));
   DBG("Units selection changed: <%s>", pud_unit2str(ed->sel_unit));

   sprite_tile_size_get(ed->sel_unit, &w, &h);
   eo_do(
      ed->bitmap,
      elm_obj_bitmap_cursor_size_set(w, h),
      elm_obj_bitmap_cursor_visibility_set(EINA_TRUE)
   );
   toolbar_actions_segment_unselect(ed);
   editor_sel_action_set(ed, 0);
}

static void
_radio_players_changed_cb(void *data,
                          Evas_Object *obj,
                          void        *event EINA_UNUSED)
{
   Editor *ed = evas_object_data_get(obj, "editor");
   _radio_changed_common_do(data, (int *)(&(ed->sel_player)));
   DBG("Player selection changed: <%s>", pud_color2str(ed->sel_player));
}

static void
_delete_cb(void        *data,
           Evas_Object *obj  EINA_UNUSED,
           void        *evt  EINA_UNUSED)
{
   Editor *ed = data;
   editor_handle_delete(ed);
}

static void
_undo_cb(void        *data EINA_UNUSED,
         Evas_Object *obj  EINA_UNUSED,
         void        *evt  EINA_UNUSED)
{
   // TODO
}

static void
_redo_cb(void        *data EINA_UNUSED,
         Evas_Object *obj  EINA_UNUSED,
         void        *evt  EINA_UNUSED)
{
   // TODO
}

static void
_minimap_show_cb(void        *data,
                 Evas_Object *obj  EINA_UNUSED,
                 void        *evt  EINA_UNUSED)
{
   Editor *ed = data;
   minimap_show(ed);
}


/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

Eina_Bool
menu_add(Editor *ed)
{
   Elm_Object_Item *itm, *i;
   Evas_Object *rd;

   ed->menu = elm_win_main_menu_get(ed->win);
   EINA_SAFETY_ON_NULL_RETURN_VAL(ed->menu, EINA_FALSE);

   evas_object_data_set(ed->menu, "editor", ed);

   /*==== FILE MENU ====*/
   itm = elm_menu_item_add(ed->menu, NULL, NULL,  "File", NULL, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "New...", _win_new_cb, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "Open...", _win_open_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Save", _win_save_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Save As...", _win_save_as_cb, ed);
   elm_menu_item_separator_add(ed->menu, itm);
   elm_menu_item_add(ed->menu, itm, NULL, "Close", _win_del_cb, ed);

   /*==== EDIT MENU ====*/
   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Edit", NULL, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "Undo", _undo_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Redo", _redo_cb, ed);
   elm_menu_item_separator_add(ed->menu, itm);
   elm_menu_item_add(ed->menu, itm, NULL, "Delete", _delete_cb, ed);

   /*==== VIEW MENU ====*/
   itm = elm_menu_item_add(ed->menu, NULL, NULL, "View", NULL, NULL);
   elm_menu_item_add(ed->menu, itm, NULL, "Show Minimap", _minimap_show_cb, ed);


   /*==== TOOLS MENU ====*/
   i = itm = elm_menu_item_add(ed->menu, NULL, NULL, "Tools", NULL, NULL);

#define RADIO_ADD(unit_, label_) \
   _radio_add(ed, rd, unit_, i, label_, _radio_units_changed_cb)

   rd = NULL; /* Unset radio group */
   rd = RADIO_ADD(PUD_UNIT_HUMAN_START, "Human Start Location");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Human Air", NULL, NULL);
   RADIO_ADD(PUD_UNIT_GNOMISH_FLYING_MACHINE, "Gnomish Flying Machine");
   RADIO_ADD(PUD_UNIT_GRYPHON_RIDER, "Gryphon Rider");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Human Land", NULL, NULL);
   RADIO_ADD(PUD_UNIT_PEASANT, "Peasant");
   RADIO_ADD(PUD_UNIT_INFANTRY, "Footman");
   RADIO_ADD(PUD_UNIT_ARCHER, "Elven Archer");
   RADIO_ADD(PUD_UNIT_KNIGHT, "Knight");
   RADIO_ADD(PUD_UNIT_BALLISTA, "Balista");
   RADIO_ADD(PUD_UNIT_DWARVES, "Dwarven Demolition Squad");
   RADIO_ADD(PUD_UNIT_MAGE, "Mage");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Human Water", NULL, NULL);
   RADIO_ADD(PUD_UNIT_HUMAN_TANKER, "Human Tanker");
   RADIO_ADD(PUD_UNIT_ELVEN_DESTROYER, "Elven Destroyer");
   RADIO_ADD(PUD_UNIT_BATTLESHIP, "Battleship");
   RADIO_ADD(PUD_UNIT_HUMAN_TRANSPORT, "Human Transport");
   RADIO_ADD(PUD_UNIT_GNOMISH_SUBMARINE, "Gnomish Submarine");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Human Buildings", NULL, NULL);
   RADIO_ADD(PUD_UNIT_FARM, "Farm");
   RADIO_ADD(PUD_UNIT_TOWN_HALL, "Town Hall");
   RADIO_ADD(PUD_UNIT_KEEP, "Keep");
   RADIO_ADD(PUD_UNIT_CASTLE, "Castle");
   RADIO_ADD(PUD_UNIT_HUMAN_BARRACKS, "Human Barracks");
   RADIO_ADD(PUD_UNIT_HUMAN_SHIPYARD, "Human Shipyard");
   RADIO_ADD(PUD_UNIT_ELVEN_LUMBER_MILL, "Elven Lumber Mill");
   RADIO_ADD(PUD_UNIT_HUMAN_FOUNDRY, "Human Foundry");
   RADIO_ADD(PUD_UNIT_HUMAN_REFINERY, "Human Refinery");
   RADIO_ADD(PUD_UNIT_HUMAN_OIL_WELL, "Human Oil Platform");
   RADIO_ADD(PUD_UNIT_HUMAN_BLACKSMITH, "Human Blacksmith");
   RADIO_ADD(PUD_UNIT_STABLES, "Stables");
   RADIO_ADD(PUD_UNIT_CHURCH, "Church");
   RADIO_ADD(PUD_UNIT_GNOMISH_INVENTOR, "Gnomish Inventor");
   RADIO_ADD(PUD_UNIT_GRYPHON_AVIARY, "Gryphon Aviary");
   RADIO_ADD(PUD_UNIT_HUMAN_SCOUT_TOWER, "Human Scout Tower");
   RADIO_ADD(PUD_UNIT_HUMAN_GUARD_TOWER, "Human Guard Tower");
   RADIO_ADD(PUD_UNIT_HUMAN_CANNON_TOWER, "Human Cannon Tower");
   RADIO_ADD(PUD_UNIT_MAGE_TOWER, "Mage Tower");

   elm_menu_item_separator_add(ed->menu, itm);

   i = itm;
   RADIO_ADD(PUD_UNIT_ORC_START, "Orc Start Location");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Orc Air", NULL, NULL);
   RADIO_ADD(PUD_UNIT_GOBLIN_ZEPPLIN, "Goblin Zepplin");
   RADIO_ADD(PUD_UNIT_DRAGON, "Dragon");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Orc Land", NULL, NULL);
   RADIO_ADD(PUD_UNIT_PEON, "Peon");
   RADIO_ADD(PUD_UNIT_GRUNT, "Grunt");
   RADIO_ADD(PUD_UNIT_AXETHROWER, "Troll Axethrower");
   RADIO_ADD(PUD_UNIT_OGRE, "Ogre");
   RADIO_ADD(PUD_UNIT_CATAPULT, "Catapult");
   RADIO_ADD(PUD_UNIT_GOBLIN_SAPPER, "Goblin Sapper");
   RADIO_ADD(PUD_UNIT_DEATH_KNIGHT, "Death Knight");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Orc Water", NULL, NULL);
   RADIO_ADD(PUD_UNIT_ORC_TANKER, "Orc Tanker");
   RADIO_ADD(PUD_UNIT_TROLL_DESTROYER, "Troll Destroyer");
   RADIO_ADD(PUD_UNIT_JUGGERNAUGHT, "Juggernaught");
   RADIO_ADD(PUD_UNIT_ORC_TRANSPORT, "Orc Transport");
   RADIO_ADD(PUD_UNIT_GIANT_TURTLE, "Giant Turtle");

   i = elm_menu_item_add(ed->menu, itm, NULL, "Orc Buildings", NULL, NULL);
   RADIO_ADD(PUD_UNIT_PIG_FARM, "Pig Farm");
   RADIO_ADD(PUD_UNIT_GREAT_HALL, "Great Hall");
   RADIO_ADD(PUD_UNIT_STRONGHOLD, "Stronghold");
   RADIO_ADD(PUD_UNIT_FORTRESS, "Fortress");
   RADIO_ADD(PUD_UNIT_ORC_BARRACKS, "Orc Barracks");
   RADIO_ADD(PUD_UNIT_ORC_SHIPYARD, "Orc Shipyard");
   RADIO_ADD(PUD_UNIT_TROLL_LUMBER_MILL, "Troll Lumber Mill");
   RADIO_ADD(PUD_UNIT_ORC_FOUNDRY, "Orc Foundry");
   RADIO_ADD(PUD_UNIT_ORC_REFINERY, "Orc Refinery");
   RADIO_ADD(PUD_UNIT_ORC_OIL_WELL, "Orc Oil Platform");
   RADIO_ADD(PUD_UNIT_ORC_BLACKSMITH, "Orc Blacksmith");
   RADIO_ADD(PUD_UNIT_OGRE_MOUND, "Ogre Mound");
   RADIO_ADD(PUD_UNIT_ALTAR_OF_STORMS, "Altar of Storms");
   RADIO_ADD(PUD_UNIT_GOBLIN_ALCHEMIST, "Goblin Alchemist");
   RADIO_ADD(PUD_UNIT_DRAGON_ROOST, "Dragon Roost");
   RADIO_ADD(PUD_UNIT_ORC_SCOUT_TOWER, "Orc Scout Tower");
   RADIO_ADD(PUD_UNIT_ORC_GUARD_TOWER, "Orc Guard Tower");
   RADIO_ADD(PUD_UNIT_ORC_CANNON_TOWER, "Orc Cannon Tower");
   RADIO_ADD(PUD_UNIT_TEMPLE_OF_THE_DAMNED, "Temple of the Damned");

   elm_menu_item_separator_add(ed->menu, itm);

   i = itm;
   RADIO_ADD(PUD_UNIT_GOLD_MINE, "Gold Mine");
   RADIO_ADD(PUD_UNIT_OIL_PATCH, "Oil Patch");
   RADIO_ADD(PUD_UNIT_CRITTER, "Critter");
   RADIO_ADD(PUD_UNIT_CIRCLE_OF_POWER, "Circle of Power");
   RADIO_ADD(PUD_UNIT_DARK_PORTAL, "Dark Portal");
   RADIO_ADD(PUD_UNIT_RUNESTONE, "Runestone");

   elm_menu_item_separator_add(ed->menu, itm);

   i = elm_menu_item_add(ed->menu, itm, NULL, "NPC's", NULL, NULL);

   RADIO_ADD(PUD_UNIT_CHO_GALL, "Cho'Gall");
   RADIO_ADD(PUD_UNIT_ZULJIN, "Zuljin");
   RADIO_ADD(PUD_UNIT_GUL_DAN, "Gul'Dan");
   RADIO_ADD(PUD_UNIT_GROM_HELLSCREAM, "Grom Hellscream");
   RADIO_ADD(PUD_UNIT_KHORGATH_BLADEFIST, "Khorgath Bladefist");
   RADIO_ADD(PUD_UNIT_DENTARG, "Dentarg");
   RADIO_ADD(PUD_UNIT_TERON_GOREFIEND, "Teron Gorefiend");
   RADIO_ADD(PUD_UNIT_DEATHWING, "Deathwing");

   elm_menu_item_separator_add(ed->menu, i);

   RADIO_ADD(PUD_UNIT_LOTHAR, "Lothar");
   RADIO_ADD(PUD_UNIT_UTHER_LIGHTBRINGER, "Uther Lightbringer");
   RADIO_ADD(PUD_UNIT_TURALYON, "Turalyon");
   RADIO_ADD(PUD_UNIT_ALLERIA, "Alleria");
   RADIO_ADD(PUD_UNIT_DANATH, "Danath");
   RADIO_ADD(PUD_UNIT_KHADGAR, "Khadgar");
   RADIO_ADD(PUD_UNIT_KURDAN_AND_SKY_REE, "Kurdan and Sky'Ree");

   elm_menu_item_separator_add(ed->menu, i);

   RADIO_ADD(PUD_UNIT_SKELETON, "Skeleton");
   RADIO_ADD(PUD_UNIT_DAEMON, "Daemon");

   /* Add a fictive radio which will be used to reset the units selection */
   ed->radio_units_reset = _radio_add(ed, rd, PUD_UNIT_NONE, NULL, NULL, _radio_units_changed_cb);
   menu_unit_selection_reset(ed);

#undef RADIO_ADD


   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Players", NULL, NULL);

#define RADIO_ADD(unit_, label_) \
   _radio_add(ed, rd, unit_, itm, label_, _radio_players_changed_cb)

   rd = NULL; /* Reset the radio group */
   rd = RADIO_ADD(PUD_PLAYER_RED, "Player 1 (Red)");
   RADIO_ADD(PUD_PLAYER_BLUE,     "Player 2 (Blue)");
   RADIO_ADD(PUD_PLAYER_GREEN,    "Player 3 (Green)");
   RADIO_ADD(PUD_PLAYER_VIOLET,   "Player 4 (Violet)");
   RADIO_ADD(PUD_PLAYER_ORANGE,   "Player 5 (Orange)");
   RADIO_ADD(PUD_PLAYER_BLACK,    "Player 6 (Black)");
   RADIO_ADD(PUD_PLAYER_WHITE,    "Player 7 (White)");
   RADIO_ADD(PUD_PLAYER_YELLOW,   "Player 8 (Yellow)");

#undef RADIO_ADD

   elm_menu_item_separator_add(ed->menu, itm);
   elm_menu_item_add(ed->menu, itm, NULL, "Map Properties...", _map_properties_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Player Properties...", _player_properties_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Starting Properties...", _starting_properties_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Units Properties...", _units_properties_cb, ed);
   elm_menu_item_add(ed->menu, itm, NULL, "Upgrades Properties...", _upgrades_properties_cb, ed);

   itm = elm_menu_item_add(ed->menu, NULL, NULL, "Help", NULL, NULL);


   return EINA_TRUE;
}

void
menu_enabled_set(Editor    *ed,
                 Eina_Bool  set)
{
   Eina_List *mi, *l;
   Elm_Object_Item *eoi;

   /* Because this is used for disabled_set() */
   set = !set;

   mi = (Eina_List *)elm_menu_items_get(ed->menu);
   EINA_LIST_FOREACH(mi, l, eoi)
      elm_object_item_disabled_set(eoi, set);
}

void
menu_unit_selection_reset(Editor *ed)
{
   elm_radio_value_set(ed->radio_units_reset, PUD_UNIT_NONE);
   ed->sel_unit = PUD_UNIT_NONE;
}


static void
_era_changed_cb(void        *data,
                Evas_Object *obj,
                void        *event_info EINA_UNUSED)
{
   Editor *ed = data;
   pud_era_set(ed->pud, elm_radio_value_get(obj));
}

Evas_Object *
menu_map_properties_new(const Editor *ed,
                        Evas_Object  *parent)
{
   Evas_Object *f, *b, *o, *grp;

   /* Frame for map era */
   f = elm_frame_add(parent);
   elm_object_text_set(f, "Tileset");
   evas_object_size_hint_weight_set(f, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(f, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(f);
   b = elm_box_add(f); /* Box */
   elm_object_content_set(f, b);
   elm_box_align_set(b, 0.0f, 0.0f);
   evas_object_show(b);

   /* Tileset item 1 (Forest) */
   o = elm_radio_add(b);
   elm_radio_state_value_set(o, PUD_ERA_FOREST);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Forest");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   grp = o;
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);

   /* Tileset item 2 (Winter) */
   o = elm_radio_add(b);
   elm_radio_state_value_set(o, PUD_ERA_WINTER);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Winter");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);

   /* Tileset item 3 (Wasteland) */
   o = elm_radio_add(b);
   elm_radio_state_value_set(o, PUD_ERA_WASTELAND);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Wasteland");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   elm_radio_group_add(o, grp);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);

   /* Tileset item 4 (Swamp) */
   o = elm_radio_add(b);
   elm_radio_state_value_set(o, PUD_ERA_SWAMP);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0.5);
   elm_object_text_set(o, "Swamp");
   elm_box_pack_end(b, o);
   evas_object_show(o);
   evas_object_smart_callback_add(o, "changed", _era_changed_cb, ed);
   elm_radio_group_add(o, grp);

   /* Default value */
   elm_radio_value_set(grp, PUD_ERA_FOREST);

   return f;
}

