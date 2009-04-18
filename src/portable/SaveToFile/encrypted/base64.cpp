#include <string>

void base64_enc_group(unsigned char *g, std::string &result, int startpos, int len = 4)
{
	for (int i = 0, j = startpos; i < 4; i++, j++)
	{
		if (i > len - 1)     result[j] = '=';
		else if (g[i] <= 25) result[j] = g[i] + 'A';
		else if (g[i] <= 51) result[j] = g[i] - 26 + 'a';
		else if (g[i] <= 61) result[j] = g[i] - 52 + '0';
		else if (g[i] == 62) result[j] = '+';
		else if (g[i] == 63) result[j] = '/';
	}
}

std::string base64_enc(const std::string &str)
{
	int i = 0, j = 0, len = str.size();
	unsigned char g2[4];
	std::string result((1 + (len - 1) / 3) * 4, '\0');

	while (len - i >= 3)
	{
		g2[0] = str[i] >> 2;
		g2[1] = ((str[i] & 0x3 /* 0b00000011 */) << 4) | str[i + 1] >> 4;
		g2[2] = ((str[i + 1] & 0xf /* 0b00001111 */) << 2) | str[i + 2] >> 6;
		g2[3] = str[i + 2] & 0x3f /* 0b00111111 */;

		base64_enc_group(g2, result, j);

		i += 3;
		j += 4;
	}
	if (i < len)
	{
		int left = len - i;
		if (left == 2)
		{
			g2[0] = str[i] >> 2;
			g2[1] = ((str[i] & 0x3 /* 0b00000011 */) << 4) | str[i + 1] >> 4;
			g2[2] = ((str[i + 1] & 0xf /* b00001111 */) << 2);
			base64_enc_group(g2, result, j, 3);
		} else {
			//left == 1
			g2[0] = str[i] >> 2;
			g2[1] = (str[i] & 0x3 /* 0b00000011 */) << 4;
			base64_enc_group(g2, result, j, 2);
		}
	}
	return result;
}

void base64_dec_group(unsigned char *g, std::string &result, int startpos)
{
	unsigned char g2[4];
	for (int i = 0; i < 4; i++)
	{
		if (g[i] == '=') break;
		if (g[i] == '/') g2[i] = 63;
		else if (g[i] == '+') g2[i] = 62;
		else if (g[i] >= 'a') g2[i] = g[i] - 'a' + 26;
		else if (g[i] >= 'A') g2[i] = g[i] - 'A';
		else g2[i] = g[i] - '0' + 52;
	}

	result[startpos] = g2[0] << 2 | ((g2[1] & 0x30 /* 0b110000 */) >> 4);
	if (g[2] != '=')
	{
		result[startpos + 1] = ((g2[1] & 0xf /* 0b001111 */) << 4) | ((g2[2] & 0x3c /* 0b111100 */) >> 2);
		if (g[3] != '=')
			result[startpos + 2] = ((g2[2] & 0x3 /* 0b000011 */) << 6) | g2[3];
	}
}

std::string base64_dec(const std::string &str)
{
	int len = str.size();
	if (len == 0 || len % 4 != 0) return std::string();
	int j = 0;

	unsigned char g2[4];
	int rlen = len * 3 / 4;
	if (str[len - 1] == '=')
	{
		rlen--;
		if (str[len - 2] == '=') rlen--;
	}
	std::string result(rlen, '\0');

	for (int i = 0; i < len; i += 4)
	{
		g2[0] = str[i];
		g2[1] = str[i + 1];
		g2[2] = str[i + 2];
		g2[3] = str[i + 3];

		base64_dec_group(g2, result, j);

		j += 3;
	}

	return result;
}
