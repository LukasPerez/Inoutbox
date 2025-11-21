# Inoutbox

1. High-level Overview
Stalker AI States

The Stalker uses a hand-written C++ state machine (no Behavior Tree, by design):

Idle

Patrol

Chase

Attack

Cooldown

Return to Patrol

Transitions (intended loop):

Idle → Patrol (on begin play / after losing target).

Patrol → Chase (player detected by sight or sound).

Chase → Attack (within attack range, cooldown ready).

Attack → Cooldown.

Cooldown → Patrol or Chase (depending on whether the target is still valid).

Chase → Patrol (after X seconds without sight or sound).

The state machine lives mainly in:

AStalkerAIController (AI logic and transitions).

UStalkerStateComponent (replicated state + cooldown, used by UI).

Core C++ Classes

AInoutboxCharacter

Base character (ALS-based) for both player and Stalker.

Owns:

UStatusComponent (health / damage, replicated).

UStalkerStateComponent (for AI stalker variants).

UWidgetComponent for world-space health / state UI.

UStatusComponent

Replicated health and max health.

Server-side damage handling.

OnRep_Health updates health bars on all clients.

UStalkerStateComponent

Replicated AI state (EStalkerState).

Replicated attack cooldown (AttackCooldownEndTime, AttackCooldownDuration).

On OnRep_… updates the Stalker’s world-space widget (state label + cooldown bar).

AStalkerAIController

C++ AI controller for the Stalker.

Finite State Machine (SetStalkerState, OnEnterState, OnExitState).

Patrol logic (MoveTo + waypoints).

Chase & attack logic (MoveToActor + cooldown).

Uses UAIPerceptionComponent with Sight + Hearing.

UHealthBarWidget (C++) + BP subclasses

Base C++ widget with:

HP bar.

Cooldown bar.

State text.

Two Blueprint subclasses for simplicity:

Player bar (HP only).

Stalker bar (state text + cooldown).

2. Features Implemented vs Spec
Implemented

State Machine (C++)

Idle, Patrol, Chase, Attack, Cooldown.

Transitions driven by perception and timers (cooldown, lose-target timer).

Patrol Between Waypoints

Stalker patrols over an array of waypoints.

Waypoints are AActor-based markers placed in the level (Blueprints).

Patrol uses MoveTo and the NavMesh.

Perception: Vision + Sound

Vision using UAISense_Sight:

Configured FOV and sight radius on UAIPerceptionComponent.

Hearing using UAISense_Hearing:

Footstep noise events emitted when the player runs.

Perception events update the “current target” in the controller.

Chase Nearest Visible Player

When one or more players are perceived:

The controller selects nearest visible player as CurrentTarget.

Chase state calls MoveToActor(CurrentTarget, AttackRange * 0.8f) server-side.

Attack with Cooldown

When within AttackRange and cooldown is ready:

State → Attack then → Cooldown.

Damage applied server-side via StatusComponent.

Cooldown:

Server sets AttackCooldownEndTime = Now + Duration.

Replicated via UStalkerStateComponent.

UI uses AttackCooldownEndTime to animate a cooldown bar (0–1) in the Stalker widget.

Lose Target After X Seconds

If no valid sight or sound updates for LoseTargetTime seconds:

Chase → Patrol.

Target cleared.

Multiplayer Correctness

All AI logic runs on the server:

State changes.

Perception updates.

Patrol / MoveTo.

Damage and cooldown.

Clients only receive replicated state & health and update UI.

Each client sees identical:

AI state.

Cooldown UI.

HP changes when the AI hits a player.

Damage System & Health Replication

Damage applied only on server.

Health replicated through UStatusComponent.

OnRep_Health updates health bars on all clients.

Simple “HP bar over AI” implemented via WidgetComponent + BP.

NavMesh + MoveTo

Uses UNavigationSystemV1 + MoveTo(...).

A NavMeshBoundsVolume is required covering the patrol area.

UI / Debug Visualization

World-space widget above Stalker:

Shows current state (string from EStalkerState).

Shows attack cooldown (progress bar).

World-space widget or viewport bar for players:

Shows HP.

Log debug:

State changes logged: [StalkerAI] ControllerName state: Old -> New.

Patrol move attempts and failures logged with coordinates & result code.

3. Outstanding Issues & TODOs

These son los puntos que faltan o están incompletos, para ser transparentes:

Attack bug / “Hit just by proximity”

Currently the attack is triggered when the player is within range; there is no dedicated attack animation or montage.

Damage is applied instantly on entry to the Attack state.

TODO:

Add an animation montage.

Sync damage with an anim notify (server-side).

Configurable AI senses (data-driven) – WIP

Perception currently configurable from the StalkerAIController / component properties (editable in BP).

The intended “data-driven” design (bonus requirement) is to move sense config into a DataAsset or similar (e.g. UStalkerSenseConfig) and load from there.

TODO:

Create a UDataAsset with fields: Sight radius, FOV, hearing range, lose-target time, etc.

Initialize UAIPerceptionComponent from that asset.

Optionally, support different configs per enemy type.

Waypoint System – Could be Improved

Currently patrol points are just an array of waypoint actors the Stalker walks through in order.

Works, but is fairly basic.

TODO:

Support loop / ping-pong modes.

Optional wait time at each waypoint.

Optional randomization within a patrol area.

Cooldown UI – Functional but Minimal

Cooldown is displayed as a bar in the Stalker’s world-space widget.

There is no extra VFX or animation beyond the bar filling/emptying.

TODO:

Add color changes (e.g. red when on cooldown).

Optional text (e.g. “CD: 1.2s”).

No Behavior Tree

Task asked explicitly for a state machine; Behavior Trees were intentionally not used.

All logic is in C++ state machine + perception rather than BT tasks / decorators.

4. How to Run (1 Server + 2 Clients)
In Unreal Editor (PIE)

Open the project in Unreal Editor.

Open the main map (e.g. ALS_DemoLevel or the test map where a BP_StalkerCharacter is placed with waypoints).

Ensure:

At least one BP_StalkerCharacter in the level.

A NavMeshBoundsVolume covering the patrol area.

In the Play dropdown:

Set Number of Players = 3.

Check Run Dedicated Server.

Click Play.

This will launch:

1 dedicated server window (log only).

2 client windows (Client 1 / Client 2).

5. How to Reproduce the AI Loop (Patrol → Detect → Chase → Attack)

Place waypoints & Stalker

In the map, place several waypoint actors (e.g. BP_Waypoint).

Assign them to the Stalker (either via a Patrol component or via exposed array in the Stalker’s controller / character BP).

Make sure all patrol points sit on the NavMesh.

Start PIE with 3 players (as above).

From Client 1, move your player into the Stalker’s patrol path.

The Stalker should:

Move between waypoints (Patrol).

On sight / hearing of your player → enter Chase.

When close enough → Attack then Cooldown.

From Client 2, observe:

The Stalker’s behaviour (patrol/chase/attack) matches what Client 1 sees.

The HP bar for the attacked player goes down identically on both clients.

Check logs (optional):

In the server’s Output Log you should see lines like:

[StalkerAI] BP_StalkerAIController_C_0 state: Patrol -> Chase

[StalkerAI] BP_StalkerAIController_C_0 state: Chase -> Attack

And patrol / MoveTo debug logs.

6. Testing Multiplayer Consistency
Verify damage consistency

Run with Number of Players = 3 and Dedicated Server.

Let the AI stalk and attack Player A (Client 1).

Watch HP on:

Client 1 (Player A’s window).

Client 2 (other client window).

Both clients must see the same HP reduction on Player A.

If one client sees different HP than the other, the problem would be in replication / OnRep; current design is built to avoid that by having all health logic server-side and replicated via UStatusComponent.

7. Simulating Lag (Net PktLag=80)

The spec asks to test with simulated lag: 

Test Task for Lukas Pérez Ibarr…

With the game running in PIE (3 players), focus one client window (e.g. Client 1).

Open the console (~).

Enter:

Net PktLag=80


This simulates roughly 80 ms of packet lag on that client.

Move the player near the Stalker and let it patrol/chase/attack.

Observe:

The Stalker movement should stay smooth (no wild stuttering / teleporting).

AI state transitions still make sense (patrol → chase → attack).

HP and cooldown UI remain consistent across both clients (only delayed by lag, not desynced).

To reset lag, you can run:

Net PktLag=0

8. Design Notes / Rationale

State Machine over Behavior Tree

The test explicitly mentions a state machine, so the core AI logic was implemented with a C++ FSM rather than Behavior Trees.

This keeps the flow very explicit and easier to read for the purpose of the task.

Server-authoritative AI

All decisions (state changes, MoveTo, attack, damage, cooldown) are made server-side.

Clients are “dumb viewers” that only render replicated position, health, state and cooldown status.

Data-driven Senses (WIP)

Senses are currently configurable via exposed properties on controller/components (FOV, radius, etc.) and will be moved to a dedicated DataAsset to fully meet the “data-driven” bonus requirement.