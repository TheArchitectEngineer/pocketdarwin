#!/usr/bin/env ruby
require 'json'
require 'time'

class PocketDarwinDiagnostic
  def initialize
    @report = {}
  end

  def collect
    @report[:timestamp] = Time.now.utc.iso8601
    @report[:system] = system_info
    @report[:uptime] = uptime
    @report[:memory] = memory_info
    @report[:disk] = disk_usage
    @report[:processes] = process_snapshot
    @report[:panic_log] = panic_log
    @report
  end

  def system_info
    {
      kernel: `uname -a`.strip,
      hostname: `hostname`.strip,
      architecture: `uname -m`.strip,
      os_release: read_file("/etc/os-release")
    }
  end

  def uptime
    `uptime`.strip
  end

  def memory_info
    vm_stat = `vm_stat 2>/dev/null`
    sysctl_mem = `sysctl -n hw.memsize 2>/dev/null`

    {
      total_bytes: sysctl_mem.strip.empty? ? nil : sysctl_mem.to_i,
      vm_stat: vm_stat.strip
    }
  end

  def disk_usage
    `df -h`.strip
  end

  def process_snapshot
    `ps aux`.lines.first(20).join.strip
  end

  def panic_log
    possible_paths = [
      "/var/log/panic.log",
      "/Library/Logs/DiagnosticReports",
      "/var/log/system.log"
    ]

    possible_paths.each do |path|
      return read_file(path, 4096) if File.exist?(path)
    end

    nil
  end

  def read_file(path, max_bytes = nil)
    return nil unless File.exist?(path)
    content = File.read(path)
    max_bytes ? content.byteslice(0, max_bytes) : content
  rescue
    nil
  end

  def save(path)
    File.write(path, JSON.pretty_generate(@report))
  end
end

if __FILE__ == $0
  reporter = PocketDarwinDiagnostic.new
  report = reporter.collect

  if ARGV[0]
    reporter.save(ARGV[0])
    puts "Diagnostic report saved to #{ARGV[0]}"
  else
    puts JSON.pretty_generate(report)
  end
end

