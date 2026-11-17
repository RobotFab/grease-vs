// Types for bubbleState.mjs, so the extension host (TypeScript) can import it.

export type BubbleState = "idle" | "compile" | "upload" | "agent" | "agentCompile" | "agentUpload";

export function bubbleOf(s: {
  uploading?: boolean;
  compiling?: boolean;
  jobBy?: string;
  agentActive?: boolean;
  agentRecent?: boolean;
}): BubbleState;
