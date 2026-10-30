# Where docs go

The files at the top of `docs/` are reference docs for users and developers. They describe the
game as it is now:

- `gaps.md`: every guess, simplification and gap.
- `custom-changes.md`: the changes that differ from X6 on purpose (the user's design choices).

Do not put docs for work in progress (plans, features, initiatives that are not done) at the top
of `docs/`. Mixed in with the reference docs, they confuse the reader. Put them in
`docs/superpowers/`:

- `specs/`: approved designs, dated (`YYYY-MM-DD-<topic>-design.md`). Do not change a spec after
  approval, except for a pointer to its status doc or an amendment that the user approves.
- `plans/`: implementation plans, dated, one file per plan.
- `status/`: one living doc per initiative (for example `user-levels.md`): the status, what is
  left, and the decisions made after the spec. Update it when a plan is done or a decision is
  made, so that a fresh agent can continue the work from it.

Copy facts from finished work into the reference docs (`gaps.md`, `custom-changes.md`, `README.md`)
only when the user asks. A merge alone is not a reason to do it.

`docs/re/` holds copies of the RE docs made by `make docs`. Do not edit them (see the project
`CLAUDE.md`, "Sources").
