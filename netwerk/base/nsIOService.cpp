NS_IMETHODIMP
nsIOService::AllowPort(int32_t inPort, const char *scheme, bool *_retval)

    int32_t port = inPort;
    if (port == -1) {
        *_retval = true;
        return NS_OK;
    }

    
    // Ensure the port number is within a valid range
    if (port <= 0 || port >= std::numeric_limits<uint16_t>::max()) {
        *_retval = false;
        return NS_OK;
    }
