local codeName
if app.TESTING then
  codeName = "TESTING"
else
  codeName = "Thoon"
end

local frameSize = app.globalConfig.frameLength
local sampleRate = app.globalConfig.sampleRate

local M = {
  Version = {
    String = "?",
    Name = string.format("%s (%0.0fkHz, %dsmps)", codeName, sampleRate / 1000.0,
                         frameSize),
    -- Name = "Dev",
    Major = 0,
    Minor = 6,
    Build = 0,
    Patch = 0,
    Status = "unstable"
  },

  EncoderThreshold = {
    Default = 3,
    SlidingList = 2,
    DurationControl = 5
  },

  EncoderSensitivity = {
    Default = 1.0,
    SampleDisplay = 0.01
  },

  Zoom = {
    In = 2,
    Out = 2
  },

  DetailPanel = {
    X = math.floor(0.5 * (app.BUTTON4_CENTER + app.BUTTON5_CENTER)) + 3
  }
}

if M.Version.Patch > 0 then
  M.Version.String = string.format("%d.%d.%02dp%d (%s)", M.Version.Major,
                                   M.Version.Minor, M.Version.Build,
                                   M.Version.Patch, M.Version.Status)
else
  M.Version.String = string.format("%d.%d.%02d (%s)", M.Version.Major,
                                   M.Version.Minor, M.Version.Build,
                                   M.Version.Status)
end

M.Version.SimpleString = string.format("%d.%d.%02d", M.Version.Major,
                                       M.Version.Minor, M.Version.Build)

return M
