%module net

%{
    #include "net.h"
%}

%rename(close) net_close;
%rename(iptoi) net_iptoi;
%rename(itoip) net_itoip;
%rename(open_udp) net_open_udp;
%rename(recv) net_recv;
%rename(resolve_host) net_resolve_host;
%rename(send) net_send;

%include <stdint.i>
%include "../../include/net.h"

