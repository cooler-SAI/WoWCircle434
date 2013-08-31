/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

#include <ace/Stack_Trace.h>
#include "Common.h"
#include "Debugging/Errors.h"
#include "Logging/Log.h"
#include "Utilities/ByteConverter.h"

class ByteBufferException
{
    public:
        ByteBufferException(bool _add, size_t _pos, size_t _esize, size_t _size)
            : add(_add), pos(_pos), esize(_esize), size(_size)
        {
            PrintPosError();
        }

        void PrintPosError() const
        {
            ACE_Stack_Trace trace;
            sLog->outError(LOG_FILTER_GENERAL, "Attempted to %s in ByteBuffer (pos: " SIZEFMTD " size: " SIZEFMTD ") value with size: " SIZEFMTD "\n%s",
                (add ? "put" : "get"), pos, size, esize, trace.c_str());
        }
    private:
        bool add;
        size_t pos;
        size_t esize;
        size_t size;
};

struct WriteAsBit
{
    explicit WriteAsBit(bool value) : m_value(value) {}
    bool m_value;
};

struct ReadAsBit
{
    explicit ReadAsBit(bool& value) : m_value(&value) {}
    bool* m_value;
};

template<uint8 BITS>
struct WriteAsUnaligned
{
    explicit WriteAsUnaligned(uint32 value) : m_value(value) { ASSERT(BITS <= 32); }
    uint32 m_value;
};

template<uint8 BITS>
struct ReadAsUnaligned
{
    explicit ReadAsUnaligned(uint32& value) : m_value(&value) { ASSERT(BITS <= 32); }
    uint32* m_value;
};

struct WriteBuffer
{
    WriteBuffer(uint8 const* buf, uint32 size) : m_buf(buf), m_size(size) { }
    WriteBuffer(char const* buf, uint32 size) : m_buf((uint8 const*)buf), m_size(size) { }
    uint8 const* m_buf;
    uint32 m_size;
};

class ByteBuffer
{
    public:
        const static size_t DEFAULT_SIZE = 64;

        // constructor
        ByteBuffer(): _rpos(0), _wpos(0), _bitpos(8)
        {
            _storage.reserve(DEFAULT_SIZE);
        }

        // constructor
        ByteBuffer(size_t res, bool init = false) : _rpos(0), _wpos(0), _bitpos(8)
        {
            if (init)
                _storage.resize(res, 0);
            else
                _storage.reserve(res);
        }

        // copy constructor
        ByteBuffer(const ByteBuffer &buf) : _storage(buf._storage), _rpos(buf._rpos), _wpos(buf._wpos),
            _bitpos(buf._bitpos)
        {
        }

        void clear()
        {
            _storage.clear();
            _rpos = _wpos = 0;
            _bitpos = 8;
        }

        template <typename T> ByteBuffer& append(T value)
        {
            EndianConvert(value);
            append((uint8 *)&value, sizeof(value));
            return *this;
        }

        void ResetBits()
        {
            _bitpos = 8;
        }

        ByteBuffer& WriteBit(bool bit)
        {
            if (_bitpos == 8)
            {
                *this << uint8(0);
                _bitpos = 0;
            }

            ++_bitpos;
            if (bit)
                _storage[_wpos-1] |= (1 << (8 - _bitpos));

            return *this;
        }

        bool ReadBit()
        {
            if (_bitpos == 8)
            {
                read_skip<uint8>();
                _bitpos = 0;
            }

            ++_bitpos;
            return (_storage[_rpos-1] >> (8 - _bitpos)) & 1;
        }

        uint32 ReadPackedTime()
        {
            uint32 packedDate = read<uint32>();

            const time_t currTime = time(NULL);
            tm* lt = localtime(&currTime);

            lt->tm_sec = 0;
            lt->tm_min = packedDate & 0x3F;
            lt->tm_hour = (packedDate >> 6) & 0x1F;
            //lt->tm_wday = (packedDate >> 11) & 7;
            lt->tm_wday = 0;
            lt->tm_mday = ((packedDate >> 14) & 0x3F) + 1;
            lt->tm_mon = (packedDate >> 20) & 0xF;
            lt->tm_year = ((packedDate >> 24) & 0x1F) + 100;

            return uint32(mktime(lt));
        }

        ByteBuffer& ReadPackedTime(uint32& time)
        {
            time = ReadPackedTime();
            return *this;
        }

        template<typename T>
        ByteBuffer& WriteBitsAt(T value, int bits, size_t pos, uint8 bitpos)
        {
            while (bits > 0)
            {
                if (bitpos == 8)
                {
                    ++pos;
                    bitpos = 0;
                }

                uint8 writtenThisCycle = 8 - bitpos;
                if (writtenThisCycle > bits)
                    writtenThisCycle = bits;

                bits -= writtenThisCycle;
                bitpos += writtenThisCycle;

                _storage[pos-1] |= ((value >> bits) & ((1 << writtenThisCycle) - 1)) << (8 - bitpos);

                // End of the extinction cycle
                // Reapers are back to work
                // Earth is dead.
            }

            return *this;
        }

        template<typename T>
        ByteBuffer& WriteBits(T value, int bits)
        {
            while (bits > 0)
            {
                if (_bitpos == 8)
                {
                    *this << uint8(0);
                    _bitpos = 0;
                }

                uint8 writtenThisCycle = 8 - _bitpos;
                if (writtenThisCycle > bits)
                    writtenThisCycle = bits;

                bits -= writtenThisCycle;
                _bitpos += writtenThisCycle;

                _storage[_wpos-1] |= ((value >> bits) & ((1 << writtenThisCycle) - 1)) << (8 - _bitpos);

                // End of the extinction cycle
                // Reapers are back to work
                // Earth is dead.
            }

            return *this;
        }

        template<uint8 BITS>
        ByteBuffer& WriteUnaligned(uint32 value)
        {
            ASSERT(BITS <= 32);

            uint32 bits = BITS;

            while (bits > 0)
            {
                if (_bitpos == 8)
                {
                    *this << uint8(0);
                    _bitpos = 0;
                }

                uint8 writtenThisCycle = 8 - _bitpos;
                if (writtenThisCycle > bits)
                    writtenThisCycle = uint8(bits);

                bits -= writtenThisCycle;
                _bitpos += writtenThisCycle;

                _storage[_wpos-1] |= ((value >> bits) & ((1 << writtenThisCycle) - 1)) << (8 - _bitpos);

                // End of the extinction cycle
                // Reapers are back to work
                // Earth is dead.
            }

            return *this;
        }

        uint32 ReadBits(int bits)
        {
            if (bits < 0 || bits > 32)
            {
                sLog->outError(LOG_FILTER_GENERAL, "ByteBuffer::ReadBits(%d) - incorrect bit count!", int32(bits));
                throw ByteBufferException(false, _rpos, bits, 32);
            }

            uint32 value = 0;
            while (bits > 0)
            {
                if (_bitpos == 8)
                {
                    read_skip<uint8>();
                    _bitpos = 0;
                }

                uint8 readThisCycle = 8 - _bitpos;
                if (readThisCycle > bits)
                    readThisCycle = uint8(bits);

                bits -= readThisCycle;
                _bitpos += readThisCycle;

                value |= ((_storage[_rpos-1] >> (8 - _bitpos)) & ((1 << readThisCycle) - 1)) << bits;
            }
            return value;
        }

        template<uint8 BITS>
        uint32 ReadUnaligned()
        {
            ASSERT(BITS <= 32);

            uint32 value = 0;
            uint32 bits = BITS;
            while (bits > 0)
            {
                if (_bitpos == 8)
                {
                    read_skip<uint8>();
                    _bitpos = 0;
                }

                uint8 readThisCycle = 8 - _bitpos;
                if (readThisCycle > bits)
                    readThisCycle = bits;

                bits -= readThisCycle;
                _bitpos += readThisCycle;

                value |= ((_storage[_rpos-1] >> (8 - _bitpos)) & ((1 << readThisCycle) - 1)) << bits;
            }
            return value;
        }

        ByteBuffer& ReadByteMask(uint8& b)
        {
            b = ReadBit() ? 1 : 0;
            return *this;
        }

        ByteBuffer& ReadByteSeq(uint8& b)
        {
            ResetBits();
            if (b != 0)
            {
                ASSERT(b == 1);
                b ^= read<uint8>();
            }
            return *this;
        }

        ByteBuffer& WriteByteMask(uint8 b)
        {
            return WriteBit(b != 0);
        }

        ByteBuffer& WriteByteSeq(uint8 b)
        {
            if (b != 0)
                append<uint8>(b ^ 1);

            return *this;
        }

        template <typename T> void put(size_t pos, T value)
        {
            EndianConvert(value);
            put(pos, (uint8 *)&value, sizeof(value));
        }

        ByteBuffer &operator<<(uint8 value)
        {
            return append<uint8>(value);
        }

        ByteBuffer &operator<<(uint16 value)
        {
            return append<uint16>(value);
        }

        ByteBuffer &operator<<(uint32 value)
        {
            return append<uint32>(value);
        }

        ByteBuffer &operator<<(uint64 value)
        {
            return append<uint64>(value);
        }

        // signed as in 2e complement
        ByteBuffer &operator<<(int8 value)
        {
            return append<int8>(value);
        }

        ByteBuffer &operator<<(int16 value)
        {
            return append<int16>(value);
        }

        ByteBuffer &operator<<(int32 value)
        {
            return append<int32>(value);
        }

        ByteBuffer &operator<<(int64 value)
        {
            return append<int64>(value);
        }

        // floating points
        ByteBuffer &operator<<(float value)
        {
            return append<float>(value);
        }

        ByteBuffer &operator<<(double value)
        {
            return append<double>(value);
        }

        ByteBuffer &operator<<(ByteBuffer const& value)
        {
            return append(value);
        }

        ByteBuffer &operator<<(const std::string &value)
        {
            return append((uint8 const*)value.c_str(), value.length()).append((uint8)0);
        }

        ByteBuffer &operator<<(const char *str)
        {
            return append((uint8 const*)str, str ? strlen(str) : 0).append((uint8)0);
        }

        ByteBuffer &operator<<(WriteAsBit value)
        {
            return WriteBit(value.m_value);
        }

        ByteBuffer &operator<<(WriteBuffer const& value)
        {
            return append(value.m_buf, value.m_size);
        }

        template<uint8 BITS>
        ByteBuffer &operator<<(WriteAsUnaligned<BITS> value)
        {
            return WriteUnaligned<BITS>(value.m_value);
        }

        ByteBuffer &operator>>(bool &value)
        {
            value = read<char>() ? true : false;
            return *this;
        }

        ByteBuffer &operator>>(ReadAsBit value)
        {
            *value.m_value = ReadBit();
            return *this;
        }

        template<uint8 BITS>
        ByteBuffer &operator>>(ReadAsUnaligned<BITS> value)
        {
            *value.m_value = ReadUnaligned<BITS>();
            return *this;
        }

        ByteBuffer &operator>>(uint8 &value)
        {
            value = read<uint8>();
            return *this;
        }

        ByteBuffer &operator>>(uint16 &value)
        {
            value = read<uint16>();
            return *this;
        }

        ByteBuffer &operator>>(uint32 &value)
        {
            value = read<uint32>();
            return *this;
        }

        ByteBuffer &operator>>(uint64 &value)
        {
            value = read<uint64>();
            return *this;
        }

        //signed as in 2e complement
        ByteBuffer &operator>>(int8 &value)
        {
            value = read<int8>();
            return *this;
        }

        ByteBuffer &operator>>(int16 &value)
        {
            value = read<int16>();
            return *this;
        }

        ByteBuffer &operator>>(int32 &value)
        {
            value = read<int32>();
            return *this;
        }

        ByteBuffer &operator>>(int64 &value)
        {
            value = read<int64>();
            return *this;
        }

        ByteBuffer &operator>>(float &value)
        {
            value = read<float>();
            return *this;
        }

        ByteBuffer &operator>>(double &value)
        {
            value = read<double>();
            return *this;
        }

        ByteBuffer &operator>>(std::string& value)
        {
            value.clear();
            while (rpos() < size())                         // prevent crash at wrong string format in packet
            {
                char c = read<char>();
                if (c == 0)
                    break;
                value += c;
            }
            return *this;
        }

        uint8& operator[](size_t const pos)
        {
            if (pos >= size())
                throw ByteBufferException(false, pos, 1, size());
            return _storage[pos];
        }

        uint8 const& operator[](size_t const pos) const
        {
            if (pos >= size())
                throw ByteBufferException(false, pos, 1, size());
            return _storage[pos];
        }

        size_t rpos() const { return _rpos; }

        size_t rpos(size_t rpos_)
        {
            _rpos = rpos_;
            return _rpos;
        }

        uint8 bitpos() const { return _bitpos; }

        void rfinish()
        {
            _rpos = wpos();
        }

        size_t wpos() const { return _wpos; }

        size_t wpos(size_t wpos_)
        {
            _wpos = wpos_;
            return _wpos;
        }

        template<typename T>
        ByteBuffer& read_skip() { return read_skip(sizeof(T)); }

        ByteBuffer& read_skip(size_t skip)
        {
            ResetBits();
            if (_rpos + skip > size())
                throw ByteBufferException(false, _rpos, skip, size());
            _rpos += skip;

            return *this;
        }

        template <typename T> T read()
        {
            ResetBits();
            T r = read<T>(_rpos);
            _rpos += sizeof(T);
            return r;
        }

        // This method does not modify _rpos.
        template <typename T> T read(size_t pos) const
        {
            if (pos + sizeof(T) > size())
                throw ByteBufferException(false, pos, sizeof(T), size());

            T val = *((T const*)&_storage[pos]);
            EndianConvert(val);
            return val;
        }

        ByteBuffer& read(uint8 *dest, size_t len)
        {
            if (_rpos + len > size())
               throw ByteBufferException(false, _rpos, len, size());

            ResetBits();

            if (len)
            {
                memcpy(dest, &_storage[_rpos], len);
                _rpos += len;
            }

            return *this;
        }

        ByteBuffer& read(std::string& dest, size_t len)
        {
            if (_rpos + len > size())
               throw ByteBufferException(false, _rpos, len, size());

            dest.clear();

            ResetBits();

            if (len)
            {
                dest.reserve(len+1);
                dest.append((char*)&_storage[_rpos], len);
                _rpos += len;
            }

            return *this;
        }

        void readPackGUID(uint64& guid)
        {
            if (rpos() + 1 > size())
                throw ByteBufferException(false, _rpos, 1, size());

            ResetBits();
            guid = 0;

            uint8 guidmark = 0;
            (*this) >> guidmark;

            for (int i = 0; i < 8; ++i)
            {
                if (guidmark & (uint8(1) << i))
                {
                    if (rpos() + 1 > size())
                        throw ByteBufferException(false, _rpos, 1, size());

                    uint8 bit;
                    (*this) >> bit;
                    guid |= (uint64(bit) << (i * 8));
                }
            }
        }

        const uint8 *contents() const { return &_storage[0]; }

        size_t size() const { return _storage.size(); }
        bool empty() const { return _storage.empty(); }

        void resize(size_t newsize)
        {
            _storage.resize(newsize, 0);
            _rpos = 0;
            _wpos = size();
        }

        void reserve(size_t ressize)
        {
            if (ressize > size())
                _storage.reserve(ressize);
        }

        ByteBuffer& append(const std::string& str)
        {
            return append((uint8 const*)str.c_str(), str.size() + 1);
        }

        ByteBuffer& append(const char *src, size_t cnt)
        {
            return append((const uint8 *)src, cnt);
        }

        ByteBuffer& append(const uint8 *src, size_t cnt)
        {
            ResetBits();
            if (!cnt)
                return *this;

            ASSERT(size() < 10000000);

            if (_storage.size() < _wpos + cnt)
                _storage.resize(_wpos + cnt);
            memcpy(&_storage[_wpos], src, cnt);
            _wpos += cnt;
            return *this;
        }

        ByteBuffer& append(const ByteBuffer& buffer)
        {
            return append(buffer.wpos() ? buffer.contents() : NULL, buffer.wpos());
        }

        // can be used in SMSG_MONSTER_MOVE opcode
        void appendPackXYZ(float x, float y, float z)
        {
            uint32 packed = 0;
            packed |= ((int)(x / 0.25f) & 0x7FF);
            packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
            packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
            *this << packed;
        }

        void appendPackGUID(uint64 guid)
        {
            uint8 packGUID[8+1];
            packGUID[0] = 0;
            size_t size = 1;
            for (uint8 i = 0;guid != 0;++i)
            {
                if (guid & 0xFF)
                {
                    packGUID[0] |= uint8(1 << i);
                    packGUID[size] =  uint8(guid & 0xFF);
                    ++size;
                }

                guid >>= 8;
            }
            append(packGUID, size);
        }

        void AppendPackedTime(time_t time)
        {
            tm* lt = localtime(&time);
            append<uint32>((lt->tm_year - 100) << 24 | lt->tm_mon  << 20 | (lt->tm_mday - 1) << 14 | lt->tm_wday << 11 | lt->tm_hour << 6 | lt->tm_min);
        }

        void put(size_t pos, const uint8 *src, size_t cnt)
        {
            if (pos + cnt > size())
               throw ByteBufferException(true, pos, cnt, size());
            memcpy(&_storage[pos], src, cnt);
        }

        void print_storage() const
        {
            if (!sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE)) // optimize disabled debug output
                return;

            std::ostringstream o;
            o << "STORAGE_SIZE: " << size();
            for (uint32 i = 0; i < size(); ++i)
                o << read<uint8>(i) << " - ";
            o << " ";
            sLog->outTrace(LOG_FILTER_NETWORKIO, "%s", o.str().c_str());
        }

        void textlike() const
        {
            if (!sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE)) // optimize disabled debug output
                return;

            std::ostringstream o;
            o << "STORAGE_SIZE: " << size();
            for (uint32 i = 0; i < size(); ++i)
            {
                char buf[1];
                snprintf(buf, 1, "%c", read<uint8>(i));
                o << buf;
            }
            o << " ";
            sLog->outTrace(LOG_FILTER_NETWORKIO, "%s", o.str().c_str());
        }

        void hexlike() const
        {
            if (!sLog->ShouldLog(LOG_FILTER_NETWORKIO, LOG_LEVEL_TRACE)) // optimize disabled debug output
                return;

            uint32 j = 1, k = 1;

            std::ostringstream o;
            o << "STORAGE_SIZE: " << size();

            for (uint32 i = 0; i < size(); ++i)
            {
                char buf[3];
                snprintf(buf, 1, "%2X ", read<uint8>(i));
                if ((i == (j * 8)) && ((i != (k * 16))))
                {
                    o << "| ";
                    ++j;
                }
                else if (i == (k * 16))
                {
                    o << "\n";
                    ++k;
                    ++j;
                }

                o << buf;
            }
            o << " ";
            sLog->outTrace(LOG_FILTER_NETWORKIO, "%s", o.str().c_str());
        }

    protected:
        std::vector<uint8> _storage;
        size_t _rpos, _wpos;
        // When reading, this value indicates how many bits of _storage[_rpos-1] are already read.
        // When writing, this value indicates how many bits are already written into _storage[_wpos-1].
        uint8 _bitpos;
};

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> const& v)
{
    b << (uint32)v.size();
    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b, std::list<T> const& v)
{
    b << (uint32)v.size();
    for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.clear();
    while (vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename K, typename V>
inline ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> const& m)
{
    b << (uint32)m.size();
    for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
    {
        b << i->first << i->second;
    }
    return b;
}

template <typename K, typename V>
inline ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
    uint32 msize;
    b >> msize;
    m.clear();
    while (msize--)
    {
        K k;
        V v;
        b >> k >> v;
        m.insert(make_pair(k, v));
    }
    return b;
}

// TODO: Make a ByteBuffer.cpp and move all this inlining to it.

template<>
inline ByteBuffer& ByteBuffer::read_skip<char*>()
{
    std::string temp;
    *this >> temp;
    return *this;
}

template<>
inline ByteBuffer& ByteBuffer::read_skip<char const*>()
{
    return read_skip<char*>();
}

template<>
inline ByteBuffer& ByteBuffer::read_skip<std::string>()
{
    return read_skip<char*>();
}

#endif
