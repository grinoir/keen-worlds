# More games for Keen Worlds

Research and discussion, 2026-10-25. Most entries remain candidates for future
support. Marooned on Mars — Galaxy is now playable locally through the arcade;
The Alphamatic 3.0 is now playable locally, with human campaign testing still
to do. The remaining previously discussed choices are The Prisoner's Dilemma
and Send in the Clorts. This list expands the possibilities beyond those three.

## Strong additions to assess

### Marooned on Mars — Galaxy version

**What it adds:** A retelling of Keen's first Mars adventure using the
Keen 5 engine. The user chose to offer it as the enhanced-graphics alternative
when selecting Keen 1 on the ship's arcade machine, outside the main Chronicle.
Its port can be assessed independently of original Keen 1 engine support.

**Local implementation:** The pinned community-fixed version runs natively
from the cabinet, with separate saves, ship-part progression and its ending.
Automated gameplay/save/ending checks pass; a complete human playthrough
remains. It stays outside the flight list and Chronicle completion count.

[Author's announcement](https://pckf.com/viewtopic.php?t=3598) ·
[Patch maintainer's bug-fix project](https://pckf.com/viewtopic.php?p=95396)

### Operation Ocflore

**What it adds:** A distinct alien destination, with its own graphics and
music. The author describes a completed project with mostly familiar enemy
behaviours and an emphasis on level design.

**Assessment:** Check palette changes, enemy alterations, installation and
the current corrected download. Our support for custom palettes is useful
existing infrastructure, but the patches still need an audit.

[Author's release announcement](https://pckf.com/viewtopic.php?p=91875)

### Suburb Shenanigans

**What it adds:** A neighbourhood adventure: an Earth destination would make
a different kind of stop from the alien worlds and space stations. The author
identifies it as a Keen 5 mod.

**Assessment:** Its connected levels and V-card doors need explicit support
and testing. Compare the author release and community fixes, especially
save/load behavior. Decide its story placement separately from Free Flight.

**Chosen next by the user.** The four-version static audit is complete:
[Suburb Shenanigans](../../research/suburb-shenanigans.md). Recommend the
unofficial v1.2 (2020-11-25); its maps match v1.1 and it carries gameplay/save
fixes. Destination **Earth**, after The Eight Accumulators.
Complete story/endings are extracted; engine port awaits audit review under
`CODEX_SUBURB.md`. The older assessment ranking below is historical.

[Author's release announcement](https://pckf.com/viewtopic.php?t=4701) ·
[Community bug-fix project](https://pckf.com/viewtopic.php?p=95396)

## Another story sequence

### Aurora Bliss → Canmore's Redemption

**What it adds:** Two further adventures that can be introduced in their
existing story order. Canmore's author identifies it as a direct sequel to
Aurora Bliss's epilogue and a Keen 5 mod inspired by science-fiction films
and series.

**Assessment:** Audit each game separately. Preserve their puzzles, altered
enemies and story transitions; reuse neither a base game's ending nor its
inventory assumptions without checking them against the mod.

[Aurora Bliss: release discussion and author's updates](https://pckf.com/viewtopic.php?p=104545) ·
[Canmore's Redemption: author's release and fixes](https://pckf.com/viewtopic.php?t=16501)

## More of a world already in the galaxy

### Return to the Shadowlands

**What it adds:** Another adventure on Gnosticus IV, extending a familiar
setting. The author's original description explores the western Shadowlands;
the release thread includes the author's version 1.07 announcement.

**Assessment:** Determine the source changes and porting effort. Choose how
another campaign on Gnosticus IV appears in navigation, rather than treating
the same planet as an unrelated new planet. Check its story order and saves.

[Author's project description](https://pckf.com/viewtopic.php?t=9993) ·
[Release thread and version 1.07 announcement](https://pckf.com/viewtopic.php?t=11095&start=30)

## Original Keen 1–3: the ship's arcade machine

**User decision, 2026-10-25:** If the originals are included, they are played
on an arcade machine in a corner of the Megarocket cabin. They are optional
games aboard the ship, with their own progress, outside the flight menu and
main Chronicle completion requirements.

The local implementation runs original Vorticons games through the pinned
native Chocolate Keen companion, with separate saves, scores and settings
for each episode. Missing or unsupported player data receives installation
and version guidance. Human play checks remain.

Under Keen 1, offer the original or the Galaxy remake, with Keen introducing
the enhanced graphics and a Duke Nukem arcade reference. Keep their progress
separate. All four choices now run natively in local testing; see the release
plan for automated validation and remaining human play checks.

See [the proposed timeline](chronicle-timeline.md) for the original games'
historical position, the arcade behavior and the new adventures' story order.

## Recommended assessment order

1. **The Alphamatic**: [native v3.0 implementation and checks](../../research/the-alphamatic.md)
   complete locally. Next: human campaign and ending checks.
2. **Marooned on Mars — Galaxy**: implemented as the cabinet's enhanced
   Keen 1 option; human playthrough remains.
3. **Operation Ocflore**, to introduce another distinct alien world.
4. **Suburb Shenanigans**, for a neighbourhood/Earth adventure.
5. **Aurora Bliss and Canmore's Redemption**, assessed as a story pair.
6. **Return to the Shadowlands**, after deciding how additional campaigns
   on an existing planet should be represented.

Continue the Prisoner/Clorts feasibility work alongside this list. The order
above is a recommendation; a patch/source audit may change it.

## Version selection matters

K1n9_Duk3's [Galaxy-mod bug-fix project](https://pckf.com/viewtopic.php?p=95396)
lists corrected downloads for several candidates. It also lists
**Kohntarkosz's Trilogy**, updated 2024-12-24. Reassess that package before
basing Prisoner/Clorts work on the older local downloads. This identifies a
version-selection task; it does not establish that every issue is fixed or
that the previous executable-version blocker has gone away.

For every selected world, verify a complete path from installation through
arrival, play, saving/loading, ending and return to the ship. Record the
specific source/download version used, and preserve the original authors'
credits and story identity.
