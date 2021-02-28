local app = app
local ply = app.SECTION_PLY
local Drawings = {
  Main = {},
  Sub = {},
  Control = {}
}

local x

x = app.DrawingInstructions()
x:hline(0, 256, 3)
Drawings.Main.HorizontalLine = x

x = app.DrawingInstructions()
x:hline(0, 256, app.GRID4_LINE1 + 2)
Drawings.Main.TitleLine = x

x = app.DrawingInstructions()
x:hline(0, 128, app.GRID4_LINE1 + 2)
Drawings.Sub.TitleLine = x

x = app.DrawingInstructions()
x:color(app.GRAY11)
x:vline(0.5 * (app.BUTTON1_CENTER + app.BUTTON2_CENTER), 0, 64)
x:vline(0.5 * (app.BUTTON2_CENTER + app.BUTTON3_CENTER), 0, 64)
Drawings.Sub.ThreeColumns = x

x = app.DrawingInstructions()
x:hline(0, 128, app.GRID5_LINE1 + 1)
x:hline(0, 128, app.GRID5_LINE3 - 1)
Drawings.Sub.HelpfulButtons = x

x = app.DrawingInstructions()
x:color(app.WHITE)
x:hline(ply - 7, ply - 3, 60)
x:hline(ply - 7, ply - 3, 56)
x:color(app.GRAY7)
x:hline(ply - 6, ply - 4, 59)
x:hline(ply - 6, ply - 4, 58)
x:hline(ply - 6, ply - 4, 57)
x:vline(ply - 5, 56, 53)
Drawings.Control.Pin = x

return Drawings
