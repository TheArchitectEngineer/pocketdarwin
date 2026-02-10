class Pdsys < Formula
  desc "PocketDarwin System bundle creator and image forger"
  homepage "https://github.com/oakymacintosh/pocketdarwin"
  url "https://github.com/OakyMacintosh/PocketDarwin/releases/download/pdsys-systembundlemkr-0.1.0/pdsys-brew-0.1.0.tar.gz"
#  sha256 "REPLACE_WITH_REAL_SHA256"
  license "MIT"

  depends_on "python@3.12"
  depends_on "qemu" => :recommended

  def install
    libexec.install "Application.py"

    venv = virtualenv_create(libexec, "python3.12")
    venv.pip_install "typer"
    venv.pip_install "rich"

    (bin/"pdsys").write <<~EOS
      #!/bin/bash
      exec "#{libexec}/bin/python" "#{libexec}/Application.py" "$@"
    EOS
  end

  test do
    system "#{bin}/pdsys", "--help"
  end
end

