/**
 * bubbleState.mjs
 *
 * What Bubble-up shows for AI Tether's state. The sidebar reads it from
 * /state, the Plotter app from plotRows.
 *
 * @param {{ uploading?: boolean, compiling?: boolean, jobBy?: string, agentActive?: boolean, agentRecent?: boolean }} s
 */
export function bubbleOf(s) {
  const agent = s.jobBy === "agent";
  if (s.uploading) return agent ? "agentUpload" : "upload";
  if (s.compiling) return agent ? "agentCompile" : "compile";
  return s.agentActive || s.agentRecent ? "agent" : "idle";
}
