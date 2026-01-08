#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

using namespace std;

// --- STRUCTS ---
struct Wolf {
    int health = 100;
    int hunger = 0;
    int energy = 100;
};

struct Item {
    string name;
    string type;
    int effect;
    Item* next;
};

struct StoryNode {
    int id;
    string description;
    string choiceA;
    string choiceB;
    StoryNode* left;
    StoryNode* right;
    bool isEnding;
};

struct GameState {
    Wolf savedWolf;
    int savedNodeId;
    Item* savedInventory;
    GameState* next;
};

struct GameEvent {
    string description;
    int priority;
    int healthEffect;
};

struct CompareEvent {
    bool operator()(GameEvent const& e1, GameEvent const& e2) {
        return e1.priority > e2.priority;
    }
};

// --- THE ENGINE CLASS ---
// This holds all your game variables in one place
struct GameEngine {
    // DATA
    Wolf player;
    Item* inventoryHead = nullptr;
    StoryNode* root = nullptr;
    StoryNode* current = nullptr;
    GameState* stackTop = nullptr;
    priority_queue<GameEvent, vector<GameEvent>, CompareEvent> eventQueue;
    
    // MESSAGES (For GUI to display)
    string currentMessage = ""; 
    bool eventActive = false;
    GameEvent activeEvent;

    // --- HELPER FUNCTIONS (Pasted from your old code) ---

    StoryNode* createNode(int id, string desc, string a, string b, bool end) {
        StoryNode* node = new StoryNode;
        node->id = id; node->description = desc; node->choiceA = a; node->choiceB = b;
        node->left = nullptr; node->right = nullptr; node->isEnding = end;
        return node;
    }

    void addItem(string n, string t, int e) {
        Item* newItem = new Item;
        newItem->name = n; newItem->type = t; newItem->effect = e; newItem->next = nullptr;
        if (inventoryHead == nullptr) inventoryHead = newItem;
        else {
            Item* temp = inventoryHead;
            while (temp->next != nullptr) temp = temp->next;
            temp->next = newItem;
        }
        currentMessage = "You found: " + n;
    }

    Item* cloneInventory(Item* originalHead) {
        if (!originalHead) return nullptr;
        Item* newHead = new Item(*originalHead);
        newHead->next = nullptr;
        Item* currOrig = originalHead->next;
        Item* currNew = newHead;
        while (currOrig) {
            currNew->next = new Item(*currOrig);
            currNew->next->next = nullptr;
            currNew = currNew->next;
            currOrig = currOrig->next;
        }
        return newHead;
    }

    void saveGame() {
        GameState* newState = new GameState;
        newState->savedWolf = player;
        newState->savedNodeId = current->id;
        newState->savedInventory = cloneInventory(inventoryHead);
        newState->next = stackTop;
        stackTop = newState;
    }

    void undoGame() {
        if (stackTop == nullptr) {
            currentMessage = "Nothing to undo!";
            return;
        }
        GameState* state = stackTop;
        stackTop = stackTop->next;
        
        player = state->savedWolf;
        inventoryHead = state->savedInventory;
        
        // Restore pointer (Manual search for this test)
        if (state->savedNodeId == 1) current = root;
        else if (state->savedNodeId == 2) current = root->left;
        else if (state->savedNodeId == 3) current = root->right;
        else if (state->savedNodeId == 4) current = root->left->left;
        else if (state->savedNodeId == 5) current = root->left->right;

        delete state;
        currentMessage = "Time rewound!";
    }

    // --- INITIALIZATION ---
    // --- INITIALIZATION (THE 20+ NODE STORY) ---
    void init() {
        srand(time(0));
        
   		
        // -------- MAIN TREE --------
        StoryNode* n1 = createNode(1,
            "Echoes After the Storm\nThe storm has ended.\nSnow covers the forest floor.\nThe wolf stands alone.\nThe pack is gone.\nCold and hunger slowly grow.",
            "Follow blood scent",
            "Howl and search",
            false);

        StoryNode* n2 = createNode(2,
            "The Frozen Stream\nA frozen stream blocks the path.\nThe ice cracks under weight.\nOne mistake could mean death.",
            "Cross the ice",
            "Take long path",
            false);

        StoryNode* n3 = createNode(3,
            "Fading Paw Prints\nOld wolf tracks appear in the snow.\nThey are faint and disappearing.\nHope remains, but danger follows.",
            "Track prints",
            "Rest",
            false);

        StoryNode* n4 = createNode(4,
            "Cold Wounds\nThe ice breaks beneath the wolf.\nFreezing water burns the skin.\nMovement becomes slow and painful.",
            "Heal wounds",
            "Ignore pain",
            false);

        StoryNode* n5 = createNode(5,
            "Hunger Without Reward\nThe hunt fails.\nNo prey is found.\nThe stomach tightens with hunger.",
            "Hunt again",
            "Rest",
            false);

        StoryNode* n6 = createNode(6,
            "See Grey Wolf\nAnother lone wolf appears.\nEyes watch carefully from a distance.\nTrust is uncertain.",
            "Calm approach",
            "Avoid and move",
            false);

        StoryNode* n7 = createNode(7,
            "Night Beneath Open Sky\nDarkness spreads across the land.\nCold grows stronger.\nSleeping could be dangerous.",
            "Stay alert",
            "Sleep",
            false);

        StoryNode* n8 = createNode(8,
            "Slow Healing\nThe wolf rests quietly.\nPain fades slowly.\nStaying too long may attract predators.",
            "Rest longer",
            "Move before predators",
            false);

        StoryNode* n9 = createNode(9,
            "Weak Prey\nA small rabbit runs through the snow.\nIt is fast and alert.\nThe hunt is risky.",
            "Chase prey",
            "Ignore prey",
            false);

        StoryNode* n10 = createNode(10,
            "Beginnings of Pack\nThe grey wolf stays close.\nTrust begins to form.\nLeadership will soon be tested.",
            "Lead calmly",
            "Assert dominance",
            false);

        StoryNode* n11 = createNode(11,
            "Sudden Ambush\nAn enemy attacks from the shadows.\nClaws and teeth clash suddenly.",
            "Fight",
            "Run away",
            false);

        StoryNode* n12 = createNode(12,
            "After Scent of Hunters\nThe air smells of metal and smoke.\nHumans are nearby.\nFew wolves survive this danger.",
            "Avoid area",
            "Investigate area",
            false);

        StoryNode* n13 = createNode(13,
            "ENDING: Death by Hunger\nNo food is found.\nStrength fades.\nThe body finally gives up.",
            "",
            "",
            true);

        StoryNode* n14 = createNode(14,
            "Growing Pack\nMore wolves gather together.\nThey look for guidance and strength.",
            "Expand territory",
            "Feed and stabilize",
            false);

        StoryNode* n15 = createNode(15,
            "Leadership Challenge\nA strong rival steps forward.\nThe pack watches closely.\nBlood may decide leadership.",
            "Fight rival",
            "Step aside",
            false);

        StoryNode* n16 = createNode(16,
            "ENDING: Death in Weak Battle\nWounds slow movement.\nThe wolf falls in combat.",
            "",
            "",
            true);

        StoryNode* n17 = createNode(17,
            "Seek Another Weak Prey\nHunger returns.\nDesperation grows.\nRisk increases with every step.",
            "Chase prey",
            "Ignore and move",
            false);

        StoryNode* n18 = createNode(18,
            "Avoid Area\nThe wolf moves carefully through the land.\nDanger is everywhere.",
            "Chase prey",
            "Ignore prey",
            false);

        StoryNode* n19 = createNode(19,
            "ENDING: Death by Hunters\nA gunshot breaks the silence.\nThe journey ends in blood and snow.",
            "",
            "",
            true);

        StoryNode* n20 = createNode(20,
            "ENDING: Alpha Wolf Survives\nThe pack remains strong.\nThe land is protected.",
            "",
            "",
            true);

        StoryNode* n21 = createNode(21,
            "ENDING: Heroic Sacrifice\nLife is given to protect the pack.\nThe name is remembered.",
            "",
            "",
            true);

        StoryNode* n22 = createNode(22,
            "ENDING: Strong Alpha\nLeadership is earned.\nLoyalty remains.",
            "",
            "",
            true);

        StoryNode* n23 = createNode(23,
            "ENDING: Lone Survivor\nThe wolf survives alone.\nFreedom comes with loneliness.",
            "",
            "",
            true);

        StoryNode* n24 = createNode(24,
            "ENDING: Endless Wanderer\nThe wolf roams the land alone.\nThe journey never truly ends.",
            "",
            "",
            true);

        StoryNode* n25 = createNode(25,
            "ENDING: Fatal Hunt\nWeakness turns the hunt deadly.\nThe wolf does not survive.",
            "",
            "",
            true);

        StoryNode* n26 = createNode(26,
            "ENDING: Frozen Hunger\nCold and hunger overcome the body.\nThe night never ends.",
            "",
            "",
            true);

        // -------- CONNECTIONS (UNCHANGED) --------
        n1->left = n2;   n1->right = n3;
        n2->left = n4;   n2->right = n5;
        n3->left = n6;   n3->right = n7;
        n4->left = n8;   n4->right = n9;
        n5->left = n9;   n5->right = n7;
        n6->left = n10;  n6->right = n11;
        n7->left = n9;   n7->right = n11;
        n8->left = n9;   n8->right = n6;
        n9->left = n12;  n9->right = n13;
        n10->left = n14; n10->right = n15;
        n11->left = n16; n11->right = n17;
        n12->left = n18; n12->right = n19;
        n14->left = n20; n14->right = n21;
        n15->left = n22; n15->right = n23;
        n17->left = n9;  n17->right = n24;
        n18->left = n25; n18->right = n26;

        root = n1;
        current = root;
    }

    // --- MAIN GAMEPLAY FUNCTION ---
    // Instead of waiting for cin, we call this when a button is clicked
    void makeChoice(int choice) {
        if (eventActive) {
            eventActive = false; // Clear event on click
            return;
        }

        saveGame(); // Auto-save

        // Move
        if (choice == 1 && current->left) {
            current = current->left;
            player.energy -= 10;
        } else if (choice == 2 && current->right) {
            current = current->right;
            player.energy -= 5;
        }

        player.hunger += 5;

        // Check Items
        if (current->id == 4) addItem("Rabbit Meat", "Food", 20);

        // Check Random Events
        int r = rand() % 100;
        if (r < 30) { // 30% chance for test
            GameEvent e = {"Sudden Snowstorm! -10 Health", 2, -10};
            eventQueue.push(e);
        }

        // Process Event if exists
        if (!eventQueue.empty()) {
            activeEvent = eventQueue.top();
            eventQueue.pop();
            player.health += activeEvent.healthEffect;
            eventActive = true; // Tell GUI to show popup
        }
    }
};

#endif