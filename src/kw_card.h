/*
Keen Worlds: the project card (quit screen).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_CARD_H
#define KW_CARD_H

// The card text (edit it in kw_card.c). The body is NULL-terminated.
extern const char *kw_cardByline;
extern const char *kw_cardTitle;
extern const char *kw_cardBody[];

// Print the card as the text-mode screen shown when the game quits.
void KW_PrintQuitCard(void);

#endif
