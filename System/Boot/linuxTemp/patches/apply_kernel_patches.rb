#!/usr/bin/env ruby
# frozen_string_literal: true

require "open3"

def run!(cmd, chdir:)
  puts "==> #{cmd.join(' ')}"
  stdout, stderr, status = Open3.capture3(*cmd, chdir: chdir)
  print stdout unless stdout.empty?
  warn stderr unless stderr.empty?
  raise "command failed (#{status.exitstatus}): #{cmd.join(' ')}" unless status.success?
end

def run(cmd, chdir:)
  stdout, stderr, status = Open3.capture3(*cmd, chdir: chdir)
  [status.success?, stdout, stderr, status.exitstatus]
end

def successful?(cmd, chdir:)
  _stdout, _stderr, status = Open3.capture3(*cmd, chdir: chdir)
  status.success?
end

kernel_dir = File.expand_path(ARGV[0] || File.join(__dir__, "..", "kernel_motorola_msm8916"))
patch_dir = File.expand_path(ARGV[1] || File.join(__dir__, "kernel_motorola_msm8916"))

unless Dir.exist?(kernel_dir)
  warn "Kernel directory not found: #{kernel_dir}"
  exit 1
end

unless Dir.exist?(patch_dir)
  warn "Patch directory not found: #{patch_dir}"
  exit 1
end

patches = Dir.glob(File.join(patch_dir, "*.patch")).sort
if patches.empty?
  puts "No patches found in #{patch_dir}"
  exit 0
end

patches.each do |patch|
  reversed_dry_run = ["patch", "-p1", "--forward", "--dry-run", "-R", "-i", patch]
  if successful?(reversed_dry_run, chdir: kernel_dir)
    puts "Skipping already-applied patch: #{File.basename(patch)}"
    next
  end

  ok, stdout, stderr, code = run(["patch", "-p1", "--forward", "-i", patch], chdir: kernel_dir)
  puts "==> patch -p1 --forward -i #{patch}"
  print stdout unless stdout.empty?
  warn stderr unless stderr.empty?
  if ok
    next
  end

  if successful?(reversed_dry_run, chdir: kernel_dir)
    puts "Skipping already-applied patch after failed forward apply: #{File.basename(patch)}"
    next
  end

  # Fallback for vendor trees where whitespace/tab drift breaks patch context.
  makefile = File.join(kernel_dir, "Makefile")
  if File.file?(makefile)
    text = File.read(makefile)
    original = text.dup

    text.gsub!("/bin/false)", "false)")

    old_block = <<~BLOCK
      \t\t/bin/false; \\
    BLOCK
    new_block = <<~BLOCK
      \t\tif [ "$${ALLOW_DIRTY_SRCTREE}" != "1" ]; then \\
      \t\t\tfalse; \\
      \t\telse \\
      \t\t\techo >&2 "  continuing due to ALLOW_DIRTY_SRCTREE=1"; \\
      \t\tfi; \\
    BLOCK
    text.gsub!(old_block, new_block)

    if text != original
      File.write(makefile, text)
      puts "Applied fallback text patch for #{File.basename(patch)}"
      next
    end
  end

  puts "No fallback changes needed for #{File.basename(patch)}; assuming already patched"
end

puts "Patch application complete."
